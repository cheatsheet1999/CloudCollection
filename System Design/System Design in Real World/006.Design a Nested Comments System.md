# Design a Nested Comments System

## System requirements

---

## Functional Requirements:

- **Hierarchical Comments**: Users can post top-level comments on a post (article, video, etc.) or reply to existing comments, creating nested threads up to 5 levels deep.
- **Anonymous Posting**: The system allows comments from anonymous users (no login required). These comments are handled similarly to registered users’ comments, but may have a placeholder author (e.g. “Guest”).
- **Thread Integrity**: The ordering of comments and the parent-child relationships must be preserved. Replies appear immediately under their parent comment in chronological order.
- **Fetching Comment Threads**: Users can retrieve the comments for a given post, with the full thread hierarchy intact (nested replies under each comment). Pagination should be supported for posts with very large numbers of comments.

---

## Non-Functional:

- **Scalability**: The system must scale horizontally to accommodate massive volumes of comments and high concurrency. It should handle spikes in usage (e.g. a viral post with millions of comments) by distributing load across servers and databases.
- **Low Latency Reads**: Fetching a comment thread (even deeply nested) should be fast – ideally on the order of tens of milliseconds – to ensure a smooth user experience. Frequent read operations must be optimized (through indexing, caching, etc.) to avoid slow page loads.
- **High Throughput Writes**: The design must sustain a high rate of new comments being posted globally. (For context, Facebook handles tens of billions of comments per day​, and Reddit sees on the order of 2–8 million comments per day​.) Our system should be able to handle hundreds of millions of new comments per day.
- **Availability & Reliability**: The service should be fault-tolerant and highly available. There should be no single point of failure – if a server or database node goes down, the system remains operational (possibly with slightly reduced capacity).
- **Consistency Model**: Strong consistency is desirable for each post’s comment thread (users see their comment immediately after posting). However, cluster-wide immediate consistency can be relaxed if needed for scalability (eventual consistency is acceptable for propagation to all users​). Real-time instantaneous synchronization is not required (no live comment streaming).

---

## Out of scope features:

- **No Real-Time Push**: The system does not need to push new comments to clients in real-time. Users will see new comments on page refresh or by manually fetching. This simplifies the design (no persistent WebSocket connections or long-poll needed).
- **No Built-in Moderation**: We assume moderation (deleting or filtering comments) is handled separately or manually, so this design focuses on storage and retrieval. (The design can be extended to support moderation flags or deletions, but it’s not a core requirement here.)

---

## Assumptions:

- Maximum nesting depth is 5 levels (replies beyond level 5 are disallowed to prevent extremely deep threads). This cap simplifies data retrieval and prevents pathological cases of deeply recursive threads.
- The comment system is attached to a generic “Post” (could be a social media post, a news article, a video, etc.). Each comment is associated with exactly one post.
- Each post can have a very large number of comments (potentially hundreds of thousands on popular content), but typical threads will be smaller.
- We assume a large user base (tens or hundreds of millions of users) with high read/write volumes, similar to major social platforms. We will design with this scale in mind.

---

# Capacity estimation

Assume on the order of 100 million daily active users. If even 20% post comments, that’s 20 million commenting users. If each posts ~5 comments on average, we’d have about 100 million new comments per day. (This is in line with major platforms; e.g., one source estimates ~1 billion comments daily for 100M active users posting ~10 comments each​.)

---

## Write Throughput

100 million comments/day ≈ 1,157 comments/second on average. Peak rates could be higher (perhaps 5x average during peak hours) – e.g. up to ~5,000 comments/sec. We should provision for spikes (a single popular post might get thousands of comments in a few minutes).

---

## Read Throughput

Reads (fetching comment threads) typically far exceed writes. For each new comment, there may be many readers viewing that post. If we assume an average post is viewed 1,000 times, 100M comments could trigger 100 billion comment reads over time. We might expect thousands of read requests per second at steady state, and significantly more for hot posts. The system will be optimized for reads (e.g., using replication and caching).

---

## Data Storage

Each comment might contain on average ~200 bytes of text content, plus metadata (IDs, timestamps, etc.). Let’s assume ~500 bytes per comment stored including overhead. At 100M comments/day, that’s ~50 GB of new data per day. Over a year, ~18 TB of data. For even larger scales (e.g. 1B comments/day​), it scales linearly (1B/day would be ~500 GB/day). We will need a storage solution that can partition data across multiple servers to handle this volume long-term.

---

## Database Size

If we keep comments indefinitely, after 5 years at 100M/day, total comments ~182.5 billion, ~91 TB of raw data (not counting indexes). Clearly, a single machine cannot hold this; we’ll need sharding or a distributed database.

---

## Index Overhead

We plan to index fields like post_id, parent_id, etc. Indexes might add ~50-100% overhead. So actual storage could be up to ~2x the raw data size.

---

## Bandwidth

Fetching a large thread (say 1000 comments) might involve sending ~1000 * 500B = 500KB of data to the client. If 1000 users do this per second, that’s ~500MB/s of outbound traffic. We should design pagination to limit payload sizes and perhaps compress responses to reduce bandwidth.

---

## Memory/Cache

We will budget memory for caching hot comment threads. If we allocate, say, 100GB of cache, that could store tens of millions of comments in memory (evicting colder ones).

These estimations confirm that our design must be distributed (multiple servers and databases), and emphasize the importance of efficient queries and caching to handle the load.

---

# API design

## POST /posts/{postId}/comments – Add a new comment on a post.

**Request Body**: JSON with fields:

- parentId (optional, if this is a reply to another comment; if omitted or null, it’s a top-level comment on the post),
- authorId or credentials (optional; if missing, the comment is anonymous),
- content (text of the comment).

**Response**: Returns the created comment’s ID and timestamp on success. On failure, returns error (e.g. invalid postId or content too long).

**Behavior**: The server will validate the postId exists, verify user if an auth token is provided, and ensure parentId (if given) is a comment under the same post. Then insert the comment into the database. The new comment should be immediately available on subsequent fetches of the thread. If the nesting level of the parentId is already 5 (max), the API will reject the reply (cannot nest deeper).

---

## GET /posts/{postId}/comments – Fetch all comments for a given post, in hierarchical order.

**Query Params**: We support pagination parameters such as page and pageSize (or limit/offset). By default, the API might return, for example, the first 20 top-level comments and their reply threads. A page parameter would fetch the next set of top-level comments (and their replies). Alternatively, we might use cursor-based pagination (e.g. a lastCommentId or timestamp to get the next chunk).

**Response**: JSON representing the comment thread. This could be an array of comment objects. Each comment object contains its fields (id, author, content, timestamp, etc.) and a list of replies (which are comment objects of its immediate children). For example:

```json
[
  {
    "id": 123,
    "parentId": null,
    "postId": 456,
    "author": "UserA",
    "content": "...",
    "createdAt": "...",
    "replies": [
      {
        "id": 124,
        "parentId": 123,
        "postId": 456,
        "author": "UserB",
        "replies": [ ... ]
      }
    ]
  }
]
```

### Behavior
The server will fetch comments for the given post from the database (potentially using cache), assemble them into the nested structure, and return. Only comments up to the allowed depth are included. Pagination may limit the number of top-level threads returned per call (the client can request further pages).

---

## GET /comments/{commentId}/replies – Optional
This could be used to load replies on demand (e.g., if the client initially only loads top-level comments and then loads deeper levels when the user expands them). It would return the comment with the given ID and its descendant thread. In many cases, the main post fetch already covers this, so this endpoint is optional depending on UI strategy.

---

## Notes
- The API will enforce content length limits on comments (to prevent excessively large comments that could affect performance).
- Anonymous comments might require an authorName in the request (for display), or we simply label them “Anonymous.”
- Rate limiting (to prevent spam or overload) is assumed to be handled by an API gateway or middleware in front of this service.

---

# Database design

## Logical Data Model
At the core, each Comment has the following attributes:

- **id**: CommentID, a unique identifier for the comment (globally unique across the system).
- **post_id**: PostID of the content item this comment is associated with.
- **parent_id**: If the comment is a reply, this points to the CommentID of its parent comment; for top-level comments, parent_id is NULL.
- **author_id**: The user ID of the commenter (null or a special value if anonymous).
- **content**: The text content of the comment.
- **created_at**: Timestamp when the comment was created.

This model forms a tree per post: all comments for a given post_id form a hierarchy via parent_id links.

---

## Basic Schema (Comments Table)
We can implement this as a relational table Comment with columns (id, post_id, parent_id, author_id, content, created_at, ...). Primary key is id. Foreign keys: post_id references a Post and parent_id references Comment.id (self-referential FK).

However, retrieving nested comments efficiently from this simple schema alone is challenging. Pure adjacency list models require recursive SQL queries or multiple round trips.

---

## Hierarchical Data Storage Strategy
Techniques for storing hierarchical data:

- **Adjacency List**: Simple inserts, slow reads.
- **Nested Set Model**: Fast reads, expensive inserts.
- **Materialized Path**: Prefix-based queries, path length grows with depth.
- **Closure Table**: Stores all ancestor-descendant pairs with depth.

---

## Closure Table Approach
Given our read-heavy workload and capped depth (5), we favor a **Closure Table** approach.

---

## Purpose of using a Closure Table
Assume a table called **CommentClosure**.

- Stores every ancestor-descendant pair with depth.
- Allows fast descendant/ancestor queries.
- Bounded write overhead due to max depth.

---

## Why adjacency list alone is not enough
- Requires recursive SQL queries or large in-memory tree builds.
- Inefficient for large or deep threads.

---

## What the closure table buys you
- **Fetch all replies under comment X**: Single indexed query.
- **Show ancestor chain for comment Y**: Single ordered query.
- **Count replies quickly**: Constant-time count query.

The gain is read-side speed and query simplicity, which is critical for a read-heavy system like comments on a news feed.


## Trade-offs

### Extra writes
Inserting a comment now means:
- One row in `Comment`
- One self-link `(C, C, 0)`
- One row per ancestor (≤ 5 in our depth-capped design) in `CommentClosure`

The write path is a little heavier but still constant-time because depth is bounded.

### More storage
Closure rows replicate information already implied by `parent_id`—this is deliberate denormalisation. We trade disk space (cheap) for lower CPU and query latency (expensive at scale).

### More complex delete / soft-delete logic
Deleting a node means removing or flagging the associated closure rows as well.

---

## Why it is the right fit here

- Depth is capped at 5 ⇒ bounded insertion overhead.
- Reads dominate writes by orders of magnitude ⇒ optimising descendant / ancestor lookups pays off.
- Queries remain simple SQL `SELECT`s that leverage composite indexes on `(ancestor_id, depth)` or `(descendant_id, depth)`, so they scale with the size of the subtree—not the size of the entire post.

**In short:** the `CommentClosure` table is a purposeful denormalisation that makes hierarchical reads deterministic and fast while keeping the additional write cost predictable and small.

---

## Closure Table Schema

We create an auxiliary table `CommentClosure` with columns:

- `ancestor_id`
- `descendant_id`
- `depth`

Each entry means there is a path of `depth` steps from the ancestor comment to the descendant comment.

Examples:
- Direct reply ⇒ depth = 1
- Self-link ⇒ `(C, C, 0)` (stored for convenience)

Using this table:

- To fetch an entire thread under a comment (or post), query all `descendant_id` where `ancestor_id = X`.
- To fetch all ancestors of a given comment, query by `descendant_id = X`.
- To get nested ordering, use the `depth` field plus insertion order or timestamps.

We keep the base `Comment` table as the source of truth for content, authors, etc., and use `CommentClosure` purely as an index to speed up tree retrieval.

---

## Table Schemas

### Comment Table Definition

```sql
CREATE TABLE Comment (
    id BIGINT PRIMARY KEY,       -- unique comment ID
    post_id BIGINT NOT NULL,     -- the post this comment belongs to
    parent_id BIGINT NULL,       -- parent comment ID (null if top-level)
    author_id BIGINT NULL,       -- user ID of author (null if anonymous)
    content TEXT NOT NULL,
    created_at DATETIME NOT NULL,
    INDEX idx_post (post_id),          
    INDEX idx_parent (parent_id),      
    INDEX idx_post_parent (post_id, parent_id)
    -- (Plus foreign key constraints if using SQL)
);
```

Notes:
- `BIGINT` allows a very large number of comments.
- IDs can be auto-increment per shard or generated by a distributed ID service.
- `idx_post` helps retrieve all comments for a post.
- `idx_parent` helps fetch direct replies.
- `idx_post_parent` optimizes queries like “top-level comments for post X”.

---

### CommentClosure Table Definition

```sql
CREATE TABLE CommentClosure (
    ancestor_id BIGINT NOT NULL,
    descendant_id BIGINT NOT NULL,
    depth INT NOT NULL,
    PRIMARY KEY (ancestor_id, descendant_id),
    INDEX idx_ancestor (ancestor_id, depth),
    INDEX idx_descendant (descendant_id, depth)
    -- (Optional foreign keys to Comment.id)
);
```

Notes:
- Composite primary key avoids duplicates.
- `idx_ancestor` optimizes subtree queries.
- `idx_descendant` supports ancestor lookups and delete operations.

---

## Insert Flow

Each new comment insertion involves:

1. Insert the comment into the `Comment` table.
2. Insert closure rows:
   - `(C, C, 0)`
   - One row per ancestor of the parent, with `depth = ancestor.depth + 1`
3. Commit both inserts atomically (transactional in SQL).

This can be implemented with a single `INSERT ... SELECT` based on the parent’s closure rows.

---

## Example

Comment `#100` replies to comment `#90`, which is a top-level comment on post `#5`.

- `Comment` table:
  - `id = 100`
  - `post_id = 5`
  - `parent_id = 90`

- `CommentClosure` entries:
  - `(100, 100, 0)`
  - `(90, 100, 1)`

If `#90` had ancestors, those would propagate as well. With depth capped at 5, we insert at most 5 ancestor rows plus the self-link per comment—manageable overhead.

---

## Data Retrieval

### Fetch all comments for a post
Options:
- Query `Comment` by `post_id` and assemble the tree in memory using `parent_id`.
- Use pagination to avoid pulling massive datasets at once.

### Fetch a specific thread
```sql
SELECT c.*, cc.depth
FROM CommentClosure cc
JOIN Comment c ON cc.descendant_id = c.id
WHERE cc.ancestor_id = Y
ORDER BY cc.depth ASC, c.created_at ASC;
```

Returns the root comment and all descendants in order.

### Fetch full threads with pagination
- Query top-level comments for a page.
- For each, fetch its subtree using the closure table.
- Cache aggressively for hot threads.

---

## Alternate Storage (NoSQL)

A NoSQL option (e.g., DynamoDB, Cassandra) could use:
- Partition key = `postId`
- Sort key = hierarchical or comment-based key

This enables fast fetch-by-post but complicates ordering and hierarchy reconstruction. Due to these complexities and consistency concerns, we proceed with a relational + closure-table design.

---

## Indexing Strategy

Key indexes:
- `Comment.post_id`
- `Comment.parent_id`
- `Comment(post_id, parent_id)`
- `CommentClosure(ancestor_id, depth)`
- `CommentClosure(descendant_id, depth)`
- Optional full-text index on `content`

Indexes increase write cost but are essential for read performance at scale.

---

## Sharding and Partitioning

### Preferred: Partition by `post_id`
- All comments for a post live on the same shard.
- Excellent locality for reads.
- Potential hotspot risk for viral posts (mitigated by high shard count or advanced splitting).

### Alternatives
- Partition by comment ID (poor locality).
- Hybrid approaches for extremely large posts (more complex).

Closure table data should follow the same partitioning strategy as comments.

---

## ID Generation

Options:
- Composite IDs (shard + local ID)
- Distributed ID generator (e.g., Snowflake)
- UUIDs (not ideal for indexing)

We assume a Snowflake-style 64-bit ID generator:
- Globally unique
- Roughly time-ordered
- No coordination required at insert time


## Caching Strategy

To achieve low-latency reads, especially for hot content, we incorporate a caching layer:

- We can use an in-memory cache (like Redis) to store recently accessed comment threads. For example, when a user fetches comments for Post #123, the service can cache the assembled comment tree (or pages of it) under a key like `PostComments:123:page1`. Subsequent requests for the same post can be served from cache, greatly reducing database load​.
- The cache entries should be carefully managed. We might cache only the first few pages of comments for popular posts, since those are most frequently viewed. Very deep or older pages might not be worth caching.
- When a new comment is posted, we must invalidate or update the cache for that post. A simple approach is to invalidate the entire post’s comment cache on any new comment (and possibly on deletion). This ensures that the next read will fetch fresh data from the DB (including the new comment). Alternatively, we could attempt to update the cached data in place (e.g., append the new comment to the cache if it’s a reply to a currently cached thread). However, given multiple app servers, cache invalidation is the safer strategy to maintain consistency.
- The cache will use an LRU or LFU eviction policy to drop posts that haven’t been accessed recently, to make room for newer hot posts. This way, trending content stays in cache for fast access.
- Consistency: We accept a tiny window of inconsistency – e.g., if two comments are posted at nearly the same time on the same post, one server might still serve slightly stale data from cache. But if we design properly (invalidate on write), we minimize this. Since real-time update isn’t required, it’s okay if a user has to refresh to see the latest comments. Many platforms indeed allow some lag for new comments to propagate​.

We maintain a self-referential `Comment` table (each comment knows its parent) and a `CommentClosure` table that stores all ancestor-descendant relationships. For example, if comment B replies to comment A (which is a top-level comment on a post), then A is an ancestor of B. The closure table will contain `(A, B, depth=1)` as well as `(B, B, depth=0)`. This design facilitates fast queries for entire comment threads at the expense of additional writes and storage​

---

# High-level design

## Component Overview

Our nested comment system will be implemented as a service-oriented architecture, with the primary components:

---

## Client Applications

Users interact via web or mobile clients. They will call the comment APIs (through an HTTP interface). For instance, when a user opens a post page, the client will call `GET /posts/{id}/comments` to retrieve the thread. When posting a comment, the client calls the `POST` API.

---

## API Gateway / Load Balancer

Incoming requests first hit a load balancer or API gateway. This distributes requests across multiple instances of the Comment Service to handle large numbers of concurrent requests. It can also handle concerns like rate limiting and authentication (for logged-in users).

---

## Comment Service (Application Servers)

This is the stateless service that implements the comment business logic. We will run many instances of this service for scalability (e.g., behind the load balancer). Each instance can handle any request (they are not tied to specific shards), and will:

- Receive API calls, parse request parameters.
- Perform necessary checks (auth, input validation, verify parent exists, etc.).
- Execute the appropriate read/write queries against the data store (possibly via a Data Access Layer).
- Use in-memory caching for reads: first check cache for the requested data, if hit return from cache, if miss query the database and then populate the cache.
- Format the response (JSON) and return to client.

---

## Database Cluster (Comment Store)

This includes both the primary storage for comments and the closure table. We will implement it as a distributed relational database. For example, it could be a sharded MySQL/PostgreSQL cluster or a NewSQL solution. The cluster is responsible for storing all comment data persistently.

We might use one logical database with multiple shards as described (sharded by `post_id`). Each shard could be a separate DB server (or a cluster for high availability). We also set up replication: each shard can have one primary (for writes) and multiple replicas (for load-balanced reads). The Comment Service can direct read queries to replicas to scale out read throughput, while writes go to primaries. Replication lag isn’t a huge issue as long as it’s short, because eventual consistency is acceptable within a few seconds​.

---

## Cache

A distributed cache (like Redis cluster) sits between the service and the database. The Comment Service interacts with it to store/retrieve cached comment threads. This cache might be partitioned by post ID as well (which aligns with our shard strategy). We ensure the cache is large enough and properly distributed to handle popular content.

---

## User/Authentication Service

(Existing infrastructure) We assume there’s a user system in place. The Comment Service will call out to an Auth service to validate user tokens for non-anonymous requests. For fetching user profiles or names to include in comment data, it might call a User Service or database. However, we could also store the author’s username in the comment data to avoid extra lookups on each comment display (denormalization for performance).

---

## Post Service

We also assume an external service or database for posts (content). The Comment Service might call it to validate that a given `post_id` exists, or to increment a comment count on the post (if we maintain such a count for quick viewing). This is outside the core scope, but worth noting integration points.

---

# Request flows

Let’s detail the sequence of operations for key use-cases:

---

## Posting a Comment (Write Path)

1. **Client Request**: A user submits a new comment via `POST /posts/{postId}/comments`. Suppose this is a reply to comment `parentId` (or null for top-level). The request hits the load balancer, which forwards it to a Comment Service instance.
2. **Validation & Prep**: The service checks the payload: ensures content is not empty and within allowed length, verifies `postId` is valid (maybe by checking a cached list of post IDs or calling Post Service if needed), and if `parentId` is provided, fetches from DB or cache to ensure that parent exists and belongs to the same `postId`. Also, if the parent’s depth is 4 (meaning the parent is at level 4), then this new comment would be level 5 which is allowed, but if parent depth is 5, it would exceed max depth – in that case, reject the comment with an error.
3. **Authentication**: If an `authorId` or auth token is present, verify it (e.g., call Auth service or JWT verification). If missing, mark the comment as anonymous (we might set `author_id = NULL` and possibly attach an `author_name` string for display if provided).
4. **Insert to Database**: The service determines which DB shard is responsible for this post’s comments (using hash of `postId`). It sends a command to that shard’s primary DB:
   - Insert the new row into `Comment`.
   - Insert rows into `CommentClosure`:
     - Always insert `(newId, newId, depth=0)` for self.
     - If `parentId` is not null, insert `(ancestor, newId, depth=ancestorDepth+1)` for each ancestor of the parent (one `INSERT ... SELECT` using the closure table)​.
     - For a top-level comment (parent null), only insert the self-link.
   - Wrap operations in a transaction; rollback on failure.
5. **Update Counts (Optional)**: Optionally increment a comment counter on the Post (asynchronously or via a lightweight update).
6. **Cache Invalidation**: After success, invalidate the cache for this post (e.g., remove `PostComments:{postId}:*` from Redis) to ensure subsequent reads are fresh.
7. **Respond to Client**: Return the new comment’s ID (and possibly timestamp or comment data).

### Performance

This write path involves writes to two tables. Closure inserts add overhead, but depth is limited (≤ 5 extra rows). Writes distribute across shards by `postId`, avoiding a single bottleneck.

### Ensuring Consistency

By performing both inserts in one transaction, we ensure either both the comment and its closure links are saved, or neither.

---

## Fetching Comments (Read Path)

1. **Client Request**: Client requests `GET /posts/{postId}/comments` (possibly with page param). Load balancer routes to a Comment Service instance.
2. **Cache Check**: Construct key like `PostComments:{postId}:page:{N}` (or entire thread). Check Redis:
   - **Cache hit**: Return cached thread (fast path).
   - **Cache miss**: Query DB.
3. **Database Query**: Identify shard by `postId`, then:
   - If requesting first page of top-level comments: query `Comment` for `post_id = {postId} AND parent_id IS NULL` with ordering and limit.
   - Fetch replies using closure table in one query:
     - `SELECT c.*, cc.depth FROM CommentClosure cc JOIN Comment c ON cc.descendant_id=c.id WHERE cc.ancestor_id IN (...)`
   - For `GET /comments/{id}/replies`, query closure directly where `ancestor_id = id`.
   - Reads can go to replicas (eventual consistency acceptable)​.
4. **Assemble Data**: Build hierarchical JSON:
   - Map `id -> comment object`
   - Attach children to parents using `parent_id`
   - Preserve chronological ordering for replies
5. **Cache Fill**: Store result in cache with TTL (or rely on invalidation-on-write).
6. **Return Response**: Serialize to JSON and return.

### Latency considerations

Cache hits return extremely fast. On cache miss, indexed joins should remain within tens of milliseconds for moderate result sets. Pagination prevents over-fetching.

---

## Other Flows

### Anonymous vs Logged-in
Posting is similar; only difference is whether `author_id` is set. Anonymous may store `author_name` if provided. Auth step is skipped.

### Deleting a Comment
(Not initially required, but consider) Deletion would require updating `Comment` (soft-delete or hard delete) and removing/updating related closure entries, plus cache invalidation. Moderation is out of scope, so not detailed.

### Count Retrieval
If UI needs only number of comments, either:
- Maintain a counter on Post / separate counter service, updated on add/delete, or
- Derive from cached thread (not ideal at scale).

---

# Detailed component design

## Comment Service

### Stateless Logic
The Comment Service is a stateless compute component. Any instance can serve any request. All instances connect to the same caching system and database cluster. This supports horizontal scaling behind a load balancer.

### Thread Assembly Module
A module assembles/disassembles hierarchical comment structures, e.g. `buildThread(postId, commentsList)` that nests by parent. Used after DB fetch to create nested JSON.

### Pagination Logic
Handle pagination parameters. Ensure stable sorting (timestamp or ID) so comments don’t jump between pages. Cursor-based pagination can use `created_at` or last-seen `id` as a pointer.

### Input Sanitization
Sanitize and/or HTML-escape comment content to prevent script injection (XSS).

### Limits
Enforce business rules such as maximum comment length and (optionally) basic per-user/IP rate limits (spam prevention, typically via gateway/middleware).

## Data Storage Layer

We utilize a Relational Database (like MySQL/Postgres or a distributed SQL like CockroachDB). As described, the database is sharded by `post_id` to distribute load. Each shard has replication for high availability. We could use a managed cloud database where sharding is automatic, or something like Vitess for MySQL which handles sharding under the hood.

---

## Connection Pooling

The service maintains a pool of connections to each database shard (or uses a data access layer that routes queries to the correct shard). We must ensure the number of open connections is managed because with many service instances and many shards, connections can grow large. We might use a proxy or multiplexing to keep it in check.

---

## Consistency and Transactions

Each comment write uses a local transaction on its shard. Cross-shard transactions are not needed because a single comment is entirely within one shard (since it’s tied to one post). This simplifies consistency. The closure table and comment table updates are in the same shard, so we can use a single transaction there.

---

## Backup and Archival

Over time, the comment data will grow huge. We should implement a strategy for old data – for example, archive comments older than X years to cold storage or a data warehouse. Operationally, we’d also set up backup routines for each shard to prevent data loss. This doesn’t directly affect design except to ensure our design supports possibly moving or splitting data in the future.

---

# Threading Model & Hierarchy Reconstruction

## Storing Depth

In the closure approach, each closure entry has a depth. However, our API output might not explicitly need depth (since we output nested JSON). The server uses depth internally to help sort or group replies. For example, when we get closure results ordered by depth, we can iterate from depth 0 outward: depth 0 is the top-level comment(s) in that subset, depth 1 are their direct replies, etc. This makes assembling the tree easier (we can maintain a map of parent->children while iterating).

---

## Using Parent Pointers for Build

Alternatively, since we store `parent_id` in each comment, we can ignore the depth from closure when building and simply use each comment’s `parent_id` to attach it to the parent. Both approaches yield the same result.

---

## Limiting Depth in Output

The client will never see more than 5 levels as per requirements. We ensure no path in the tree exceeds this. This means our assembly code can assume that it won’t go into infinite recursion. We can even implement the assembly with a simple recursive function or iterative approach that stops at depth 5.

---

## Large Threads (Flattening Consideration)

If a particular thread is extremely large (e.g., one comment has 10,000 replies which themselves have replies…), building a huge nested structure could be heavy on the server and slow to send. To mitigate this, we might not always send all nested replies in one go. For example, Reddit often collapses very deep or very long branches by default and requires the user to click “show more.” Our API could similarly choose to cap the number of replies returned per comment and indicate if there are more.

However, given we allow depth 5 max, it’s mostly breadth (number of siblings) that could be large. We will rely on pagination at the top level and maybe partial loading for replies if needed. For now, the design assumes a single request will return all nested replies under those top-level comments up to depth 5. That should be fine as depth is limited and the number of nodes returned per page can be controlled.

---

# Pagination Design

## Top-Level Pagination

We will page at the level of top-level comments. E.g., page 1 returns first 20 top-level comments and all their nested replies; page 2 returns next 20 top-level comments, etc. This way, we never omit some replies when we have shown a parent (i.e., if a top-level comment is shown, we show all its replies to maintain context). This is a user-friendly approach – you scroll and load more root comments, each comes with its replies.

---

## Reply Pagination (optional)

If a single comment’s reply list is extremely long (say a top-level comment has 1000 direct replies), it might be useful to paginate at the reply level as well (e.g., “show more replies” for that comment). Our API design could support fetching replies of a specific comment (via `GET /comments/{id}/replies` as mentioned). This would allow lazy loading of deep parts of the thread. We will not deeply design this as it complicates things, but it is an extension.

---

## Keyset Pagination

To avoid issues with offset/limit on rapidly changing data, we might use keyset pagination. For example, pass the last seen comment’s timestamp or ID to get the next set. Since comment order is chronological (assuming that for simplicity), we can use `created_at` or `id` (if roughly time-ordered) as a cursor. This ensures stable pagination even if new comments arrive (they would appear in the end). However, given eventual consistency, a newly added comment might not show up until cache invalidation triggers a refetch. That’s acceptable.

---

## Cache and Pagination

We could cache each page separately. E.g., `PostComments:123:page1`, `...:page2`, etc. However, if new comments are inserted at the top (which would be at the end of list if chronological ascending; if we sorted descending, new comments would appear first which complicates pagination slightly), it could shift pages.

To keep it simple, we assume chronological order (oldest first) so new comments append to the end – thus they don’t affect earlier pages. If we sort newest first, we’d have to handle page 1 being invalidated often as new comments come in front. Many forums choose oldest-first for that reason in long threads. We’ll assume oldest first ordering within each level.

---

## Depth vs Breadth Pagination

We do not paginate by depth; depth is fixed at 5. Instead, we handle breadth (number of threads). This covers likely use cases.

---

# Scalability and Horizontal Scale

## Adding More Servers

The design supports adding more Comment Service instances behind the load balancer as traffic grows. Since state is in the DB and cache, new instances can pick up requests immediately.

---

## Sharding Database

As comment volume grows, we can add more shards. We’d have a shard mapping service or consistent hashing to map `postId` to shards. If the number of posts is huge, consistent hashing helps to distribute without a central directory. If we go with a simpler modulo hashing, changing number of shards is more involved (would need rebalancing).

We might abstract this with an indirection (like a lookup table that maps a range of postIds to a shard). The infrastructure team can split a shard by updating this mapping (e.g., route half the postId range to a new server) and migrating data. Such re-sharding is complex but doable offline or during low traffic.

---

## Read Replicas

For each shard, to handle heavy read load (which can be way higher than writes), we add multiple read replicas. The Comment Service can be configured to query replicas for read operations. We must ensure the replica lag is small; given a high write volume, careful tuning or use of eventually-consistent reads might be needed.

For instance, using MySQL async replication might result in seconds of lag under heavy load. If that’s an issue, we could consider a distributed DB that handles replication more gracefully or even a multi-leader setup. Or accept that a comment may not immediately show up for others for a brief moment.

---

## NoSQL or Search Engine for Scale (optional)

If the relational approach hits limits, an alternative is to use a system like ElasticSearch for retrieving comment threads (treating it like a search by postId and sorting by timestamp). Or use a wide-column store like Cassandra where each postId is a partition and comments are clustered by parent and time. Those could scale further horizontally. However, they introduce eventual consistency and complexity in maintaining hierarchical relations.

Our primary design sticks to a sharded SQL which can handle quite large scale (Facebook itself uses a lot of MySQL sharding in their backend along with a caching layer​

---

# Trade offs/Tech choices

Designing a nested comment system at scale involves several trade-offs:

---

## Relational vs NoSQL

We chose a relational model for clarity and ease of querying hierarchical relationships. This gives us ACID transactions to maintain consistency (important to keep threads intact) and powerful querying (like joins for closure). The trade-off is that a single SQL server can become a bottleneck at extreme scale, which we mitigate via sharding.

A NoSQL approach (like DynamoDB single-table design) would scale writes and storage more easily but complicates queries. For example, grouping all comments by post in one partition makes fetching easy​, but you still need to reconstruct the hierarchy on the application side. Also, very large partitions (for extremely popular posts) can be problematic in NoSQL.

We accept a bit more complexity in scaling the relational DB in exchange for simpler query logic. In future, if relational starts to strain, we could offload some data to a cache or search system, or consider NewSQL databases that scale out SQL horizontally.

---

## Closure Table vs Adjacency List Only

Using only adjacency list (parent pointers) is simpler on writes (only one insert) but reads would rely on recursive queries or multiple round-trips, which would be slow for deep or large threads​. By adding the closure table, we significantly speed up reads at the cost of additional storage and write complexity​.

The closure table essentially denormalizes the transitive relationships for fast lookup, a classic space-time trade-off​. We chose closure to optimize for read-heavy usage. The disadvantage is every comment write now does a few more writes. We decided this is acceptable given read to write ratio is high.

If writes become a bottleneck, one could consider dropping closure for simpler writes and relying on caching or more powerful queries (like Postgres recursive CTE) for reads, but at our scale that likely wouldn’t be sufficient.

---

## Nested Set or Materialized Path Alternatives

The nested set model offers fast reads (single query subtree fetch) similar to closure, but updates require shifting a large portion of the tree​. In a high-write environment, that would lock rows and be difficult to maintain.

Materialized path is simpler than nested set for inserts (string append), but querying descendants requires prefix matching, paths get long with depth, and it’s less flexible for depth queries. We found closure to be the most robust for varying query patterns​.

Storage overhead: closure table can have as many rows as number of comments times average depth. With depth cap 5, worst-case is ~N*5 closure rows plus N comment rows. We accept this overhead.

---

## Cache Consistency vs Freshness

We use a cache to improve performance, but caches introduce potential staleness. We decided on an invalidate-on-write strategy to keep things simple and mostly consistent. The risk is if cache invalidation fails or is delayed, some stale data might be served. To minimize issues, our cache entries have short TTLs as a backup.

By focusing on eventual consistency, it’s okay if a comment appears a few seconds later for some. This trade-off improves read latency at the cost of a small stale window.

---

## One Big Request vs Many Small (for threads)

Fetching all comments at once is heavy for large threads. Lazy loading reduces initial payload but increases request count and client complexity.

Our approach is hybrid:
- Paginate top-level comments.
- Fetch full sub-threads for the top-level comments returned on that page.

This balances bandwidth, context, and complexity.

---

## Depth Cap vs No Cap

We cap depth at 5 for predictability and simpler retrieval. Without a cap, closure table worst-case could grow large (e.g., a chain of depth 1000 yields 1000 ancestor links for the last node). The trade-off is flexibility vs predictable performance. We choose predictability.

---

## Using IDs vs storing object references

We store foreign keys (`post_id`, `parent_id`) rather than embedding post/user data. Normalization avoids duplication but can require extra user lookups. A common denormalization is storing the author’s username in the comment record to avoid calling user service during reads. We lean towards denormalizing author name for read efficiency​.

---

## Failure vs Complexity

We aim for graceful degradation. If cache is down, reads hit DB (slower but functional). If a shard is down, rely on replicas or cached data. We avoid overly complex distributed consistency mechanisms to keep latency low and operations understandable.

---

# Failure scenarios/bottlenecks

It’s important to consider how the system behaves under various failure conditions and identify potential bottlenecks:

---

## Failure Scenarios

### Server Instance Failure
If a Comment Service instance crashes, the load balancer stops routing to it. Because the service is stateless, requests fail over to other instances. Capacity drops slightly until replacement. No user data is lost since it’s in the DB.

### Database Node Failure
If a shard’s primary fails, writes pause until failover. With replication + automatic failover, a standby replica becomes primary. During failover, writes might error; service can retry or return an error. Reads can use other replicas; if all replicas fail, that shard’s posts become temporarily unavailable (mitigated with multi-zone replicas).

### Network Partition
If service loses DB/cache connectivity, requests fail. Use timeouts and circuit breakers. If only some shards are unreachable, only those posts are affected. Client can show “could not load comments, try again”.

### Cache Failure
If cache goes down, cache misses hit DB for all reads, increasing DB load and latency. Mitigations:
- Redundant cache nodes
- Degraded mode (smaller page sizes, throttling)
- Scale cache quickly

### Inconsistent Cache (race conditions)
Invalidate-on-write avoids complex concurrent cache updates. Worst case: an invalidation removes freshly repopulated data, causing next read to hit DB again. Small, acceptable window.

### Data inconsistency between Comment and Closure tables
Transactions prevent partial writes. We can also run periodic consistency checks as maintenance.

---

## Bottlenecks

### Hot Post Concentration
A single viral post can overload its shard (all reads/writes map to one shard). Mitigations:
- Cache (serve reads without DB)
- Request coalescing (avoid thundering herd)
- Throttling / queuing writes (extreme cases)
- Advanced partitioning for a single post (complex; last resort)

### Long Threads Rendering
Pagination prevents sending tens of thousands of comments at once. Users can load pages sequentially.

---

## Load and Bottleneck Analysis

### Database Load
DB is the critical resource. Sharding reduces per-shard load. Cache reduces read pressure. Proper indexing and selecting only needed columns helps. Closure joins are O(n) in subtree size; pagination + depth cap keep n manageable.

### Cache Memory Bottleneck
We cache only hot posts (LRU/LFU eviction). Typical long-tail distribution means caching provides significant benefit.

### Network Bandwidth
Large threads mean large payloads. Compress JSON responses. Optionally use CDN edge caching for short TTL windows (adds staleness but can absorb huge read traffic).

### Service CPU
Assembling JSON and sorting can be CPU-heavy for large responses. Mitigate with efficient libraries and response size limits. Scale out service instances.

In summary, the main bottlenecks are at the data layer. We address them with sharding, indexing, and caching. Remaining hotspots can be mitigated with targeted strategies.

---

# Future improvements

Our design covers the core functionality. Possible future work:

---

## Upvotes/Downvotes (Likes)
Add vote tracking (per-comment tally + per-user vote records). Add APIs like `POST /comments/{id}/vote`. Supporting “Top” sorting adds new query patterns and indexes.

---

## Notifications
On reply creation, enqueue an event to a Notification Service to notify the parent’s author (if registered). Anonymous users likely won’t be notified.

---

## Editing Comments
Add an API to update comment content. Update comment row, set `edited=true`, `updated_at`. Invalidate cache for the post/thread. Closure table unchanged.

---

## Optimize Closure Table Storage
Potentially drop self-links or pack depth more efficiently if space becomes an issue (at cost of query complexity).

---

## Graph Database Alternative
Considered only if graph queries become a core feature; likely overkill for this use case.

---

## Refinement of Sharding
Implement dynamic shard rebalancing and data migration during off-peak hours.

---

## Public API or Integration
Expose a public comment API with rate limits, or multi-tenant support for embedding across sites (like Disqus). Out of scope for current design.
