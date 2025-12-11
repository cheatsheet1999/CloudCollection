# Introduction to Modern System Design

Get an overview of the topics we’ll cover in this course.

---

## We'll cover the following

- What is system design?  
- Modern system design using building blocks  
- About this course  
- Who should take this course?  
- Prerequisites for this course  

---

# What is system design?

System design is the process of defining components and their integration, APIs, and data models to build large-scale systems that meet a specified set of functional and non-functional requirements.

System design uses the concepts of computer networking, parallel computing, and distributed systems to craft systems that scale well and are performant. Distributed systems scale well by nature. However, distributed systems are inherently complex. The discipline of system design helps us tame this complexity and get the work done.

```
System design
Computer networking
Distributed systems
Parallel computing
```

System design aims to build systems that are **reliable**, **effective**, and **maintainable**, among other characteristics.

- **Reliable systems** handle faults, failures, and errors.  
- **Effective systems** meet all user needs and business requirements.  
- **Maintainable systems** are flexible and easy to scale up or down. The ability to add new features also comes under the umbrella of maintainability.  

---

# Modern system design using building blocks

We have separated out commonly-used design elements, such as load balancers, as the basic building blocks for high-level system design.  

This serves two purposes:

1. It allows us to discuss all the building blocks in detail and explore their interesting mini-design problems.  
2. When we tackle a design problem, we can concentrate on problem-specific aspects while referring to the building blocks by name instead of re-explaining them every time.  

We have identified **sixteen building blocks** that are crucial in designing modern systems.

```
Building Blocks
Modern System Design
Modern systems are designed using fundamental building blocks
```

---

# About this course

This course is about designing systems that scale with increasing users, remain available even under different faults, and meet functional goals with good performance. Real-world system building is an iterative process where we start with a reasonably good design, measure how it performs, and improve the design in the next iteration.

The focus of this course is to dive into carefully selected system design problems to prepare you for both interviews and real engineering tasks. The course emphasizes **conceptual understanding** rather than memorizing boilerplate designs.

### A fresh look at system design

Many system design courses teach rigid formulas. While helpful in interviews, formulas often encourage memorizing solutions instead of understanding problems.  
System design is **both an art and a science**, and solving problems from first principles gives a more robust and adaptable foundation.

### Going deep and broad

- We revisit traditional problems, but with **in-depth discussions** and clear rationales behind every design choice.  
- We explore new problems involving not only scalability but also **availability**, **maintainability**, **consistency**, and **fault tolerance**.  
- We scope problems realistically by making appropriate assumptions, mirroring real-world engineering.  

### Iterative process

Real systems evolve. We may start simple, but bottlenecks inevitably appear and force redesigns.  
We encourage at least two iterations during design:

1. First: produce the best possible design (80% of the effort)  
2. Second: refine and improve the design  

Sometimes, improvements occur naturally (as insights appear mid-design).

### Interactive learning

- Many problems guide learners step-by-step through system design.  
- Others require designing an entire system end-to-end.  
- Questions and quizzes reinforce important concepts.

---

# Who should take this course?

System design is for **any software engineer who wants to advance in their career.**

### Interview preparation

System design interviews are increasingly common. This course includes a detailed guide on preparing for them.

### Software developers

Primarily useful for **backend developers** aiming to become **principal engineers** or **solution architects**, but equally valuable for:

- Full-stack developers  
- Frontend developers seeking backend awareness  
- SREs / DevOps engineers working in production environments  

### Product / Project managers

Managers responsible for planning scalable systems benefit greatly from understanding system design concepts.

### System design learners

Anyone in the tech industry can benefit from system design knowledge, whether:

- Understanding how large companies build scalable apps  
- Turning an idea into a real large-scale system  

---

# Prerequisites for this course

We assume familiarity with **distributed systems fundamentals**.

System design borrows many concepts from distributed systems. We have an excellent course for learners who wish to revisit those fundamentals.

Basic knowledge of:

- Computer networking  
- Operating systems  

…is also helpful before taking this course.

---


