# What Is a System Design Interview?

Learn about system design interviews (SDIs) and how to strategically approach them.

---

## We'll cover the following

- How are SDIs different from other interviews?  
- How do we tackle a design question?  
- Present the high-level design  
- Possible questions for every SDI  
- The design evolution of Google  
- Design challenges  
- The responsibility of the designer  
- Who gets a system design interview?  
- Theory and practice  

---

# Theory and practice

Our system design course is equally useful for people already working and those preparing for interviews.  
In this chapter, we highlight different aspects of a system design interview (SDI) and provide helpful tips.  
Even if you’re not preparing for interviews, many of these concepts apply broadly.

---

# How are SDIs different from other interviews?

System design interviews are unlike coding interviews.  
There is rarely any coding involved.

```
Other Interviews                   System Design Interview
```

An SDI takes place at a **much higher level of abstraction**.  
We determine requirements and map them to computational components and high-level communication protocols.

**The final answer doesn’t matter.**  
The interviewer evaluates:

- your reasoning  
- how you explore the problem  
- how you communicate trade-offs  

> System design aligns more closely with real-world engineering work than coding interviews.

---

# How do we tackle a design question?

Design questions are intentionally **open-ended** and **vague**, reflecting real-world product ambiguity.

Interviewers often choose well-known systems—for example, *designing WhatsApp*.  
But the real WhatsApp has many features. Including all of them is not practical because:

1. We have limited interview time.  
2. Demonstrating core functionality is enough to show problem-solving ability.  

We can explicitly tell the interviewer what is **in scope** and **out of scope**, and adjust based on their feedback.

### Best practices for system design interviews

- **Solidify the requirements**  
- **Scope the problem**  
- **Engage the interviewer**

Applicants must ask clarifying questions, narrow the design reasonably, and maintain active communication.

SDIs usually last **35–40 minutes**, so collaboration and transparency are key.

---

# Present the high-level design

Common high-level components include:

- Frontend  
- Load balancers  
- Caches  
- Services  
- Data processing layers  
- Storage  

Architectural diagrams typically use **boxes** for components and **arrows** for interactions.

```
[Users] → [Load balancer] → [Services] → [Cache] → [Data Storage]
```

We draw a diagram like this and walk the interviewer through it.

---

# Possible questions for every SDI

Interviewers often ask about:

- number of users  
- expected queries per second  
- how performance changes as traffic grows  

A common rule in systems engineering:

> When some parameter grows by a factor of **10**, the design might need to change.

Larger systems don't scale linearly; they require new strategies and optimizations.

---

# The design evolution of Google

Early Google Search appears simple today, but at the time it was highly sophisticated and—critically—cost-efficient.  
Every design choice has business implications.

As designers, we must use resources efficiently while meeting (or exceeding) customer needs.

---

# Design challenges

Reasons design problems are hard:

- There is **no single correct solution**.  
- Every design depends heavily on **assumptions**.  
- Systems evolve, break, and require continuous adaptation.  

---

# The responsibility of the designer

Designers must provide **robustness at the architectural level**.

Modern systems rely on millions of off-the-shelf components.  
Something is always breaking, and we must **hide this reality from the customer** through resilient design choices.

---

# Who gets a system design interview?

Traditionally:

- **Mid-level to senior** engineers (2+ years experience)  
- Senior candidates often get **2–3** SDIs  

Recently:

- Even **junior engineers** are asked system design questions at large companies  
- Learning SD early accelerates career growth  

---

# Theory and practice in system design

System design theory is rooted in **distributed systems**.  
A refresher is valuable before diving deeper.

Distributed systems concepts give us mature engineering principles:

- **Robustness**  
- **Scalability**  
- **Availability**  
- **Performance**  
- **Extensibility**  
- **Resiliency**  

Shared terminology becomes a **lingua franca** between interviewer and candidate.

For example:

> “We must trade off **availability vs consistency** under network partitions due to the **CAP theorem**.”

Using this terminology demonstrates understanding of both **theory and practice**.

---

**Remember:** It’s the candidate’s responsibility to *showcase* their design thinking.

---

