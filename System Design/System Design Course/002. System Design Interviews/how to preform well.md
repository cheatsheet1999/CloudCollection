# How to Perform Well

Learn helpful tips on how to perform effectively during a system design interview.

---

## We'll cover the following

- What to do during the interview  
- Strategize, then divide and conquer  
- Ask refining questions  
- Handle data  
- Discuss the components  
- Discuss trade-offs  
- What not to do in an interview  

---

# What to do during the interview

Candidates should avoid appearing unoriginal.  
Interviewers have likely heard the same problem many times, so reproducing a generic design won’t be impressive.

At the same time, interviews are stressful.  
Having a **structured plan** is useful. Different candidates may use different strategies, but the following approach is widely effective.

```
The dos of a system design interview
```

---

# Strategize, then divide and conquer

We recommend incorporating the following activities during the interview:

- Ask refining questions  
- Handle the data  
- Discuss the components  
- Discuss trade-offs  

```
Activities to include in the interview
```

---

# Ask refining questions

Understanding the problem and requirements is essential.  
Your goal is to take the interviewer on a clear reasoning journey that explains why your design makes sense.

SDIs test whether you can logically derive a system from vague, incomplete requirements.

To clarify requirements, categorize them into:

1. **Client needs**  
   - e.g., ability to send messages in near real time  

2. **System needs**  
   - e.g., performance must not degrade as user load increases  

> Professionals often call these *functional* and *non-functional* requirements.

---

# Handle data

We must identify the **data characteristics** to determine the appropriate systems and components.

Questions to ask yourself:

- What’s the **data size** right now?  
- At what rate will the data **grow**?  
- How will the data be **consumed** by subsystems or end users?  
- Is the data **structured or unstructured**?  
- Do we need **strict consistency**, or is **eventual consistency** acceptable?  
- What level of **durability** is required?  
- Are there **privacy or regulatory** requirements for storing or transmitting user data?  

---

# Discuss the components

```
Components
```

A key part of our job is determining:

- which components to use  
- where to place them  
- how they interact  

Example:  
Should we use a **traditional database** or a **NoSQL database**?

If the interviewer insists on a traditional database but we preferred NoSQL, what should we do?

```
As designers, we’d have a harder job because we’d need to use a traditional database and do extra work to ameliorate the shortcomings or challenges. In this case, we’d have invented a new component.
```

Such moments are excellent opportunities to demonstrate design flexibility.

> Note: We typically abstract components as boxes and show arrows to define interactions.  
> Defining user-facing APIs can also clarify system behavior.

Examples of components:

- Frontend  
- Load balancers  
- Caches  
- Databases  
- Firewalls  
- CDNs  

---

# Discuss trade-offs

There is **no single correct answer** in system design.

If two teams solve the same problem, they may design completely different systems.

Reasons include:

- Different components have different strengths and weaknesses  
- Each choice has costs (money, maintenance complexity, etc.)  
- Every system has **weak points**; good designers are aware of them  
- Trade-offs are unavoidable  

We should explicitly call out weaknesses and justify them:

> Example:  
> “This design cannot handle 10× more load right now, but current usage projections show we won’t reach that scale for years. We will monitor load growth and redesign when needed.”

Remember:  
Something is **always** failing in large systems.  
Fault tolerance and security must be integrated into the design.

---

# What not to do in an interview

Avoid the following:

- ❌ Don’t write code  
- ❌ Don’t start building without a plan  
- ❌ Don’t work in silence  
- ❌ Don’t state numbers without reasoning or context  
- ❌ Don’t pretend to know something you don’t  

```
The don'ts of a system design interview
```

If the interviewer asks about a system you’ve never heard of, be honest.  
They will explain it or change the question.

---

**Back**  
*How to Prepare for Success*

**Mark As Completed**

---

# Next  
**Let AI Evaluate your System Design Interview Preparation**
