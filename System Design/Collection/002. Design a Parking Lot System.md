# **Design an Efficient Parking Lot System**

## **System requirements**

### **Functional**
- User can reserve a parking spot.  
- User pays for the reservation.  
- User can park a car on the parking spot.  
- User can leave before the reservation time expires.  
- One common error case to handle is when a user makes a reservation, but fails to show up. In this case, we would charge for the first 24 hours.

### **Non-Functional**
- Scalability. We are designing this for an international company who has 1000s of parking lots across nations.  
- Availability.  
- Consistency. Once a reservation is made, the parking spot must be available for the user. No double-booking.  

[Generally speaking, you would like to keep the requirements scope small. You only have 35 - 50 min in an interview. If you have a lot of requirements, you'd risk running out of time. We could add many requirements here: vehicle type, indoor parking vs outdoor parking, additional services like car wash, and parking cars near the entrance to have a quick enter & exist experience, etc. But we will start with a small set of requirements. Easier to expand later than shrink.]

---

## **Capacity estimation**

I assume:

- The company has operations in 10 countries.  
- The company has 100 parking lots, on average, in one country.  
- On average, each parking lot has a capacity of 200 cars.  
- 80% of users use it for short term parking, averaging 4 hours.  
- 20% of users use it for long term parking, averaging 5 days.  
- Each parking lot has 200 parking spots on average.  
- Each parking lot receives 200 reservation requests per day.

Estimation:

- 10 * 100 = 1000 parking lots.  
- The system receives 200 * 1000 = 200K reservation requests per day.  

Each reservation would mainly consist of:

- Reservation ID (8 bytes)  
- User ID (8 bytes)  
- Car type (1 byte)  
- Reservation start time (8 byte)  
- Reservation end time (8 byte)  

Roughly speaking, let's assume this would total 128 byte in total.

Each day:  
`200K * 128 = 25.6MB`

In 2 years:  
`25.6 * 365 * 2 * 1.5 = 280 GB` (assuming future growth)

[You do not need to provide this much detail in an actual interview. The primary thing you are trying to establish is that the size requirement is small enough for RDB.]

Conclusion:

- Storage requirement is within RDB’s capability.  
- RDB provides strong consistency → important to prevent double booking.  
- Response time does not need to be sub-millisecond; a couple seconds is fine.

---

## **API design**

### **APIs used by users**

- **check_capacity(lot_ID, vehicle_type, start_date_time, end_date_time)**  
  Returns the number of open spots in the given parking lot. It also returns the price.

- **reserve_spot(user_ID, lot_ID, vehicle_type, start_date_time, end_date_time)**  
  Returns the reservation ID and the price.

- After reserve_spot(), user is forwarded to 3rd party payment provider (PayPal, credit card).  
  User receives a payment token.

- **complete_reservation(user_ID, reservation_ID, payment_token)**  
  Verifies payment token and finalizes reservation.

### **APIs used by parking lot gate service**
- **vehicle_arrived(reservation_ID, date_time)**  
- **vehicle_left(reservation_ID, date_time)**  

---

## **Database design**

[Mid-level deep dive topic.]

### **Reservation table**
- reservation_ID (primary key)  
- spot_ID (foreign key)  
- user_ID (foreign key)  
- start_time  
- end_time  
- payment_status (paid, unpaid, canceled)  
- completion_status (to be completed, fulfilled, canceled)  

### **Spot table**
- spot_ID (primary key)  
- lot_ID (foreign key)  

### **Lot table**
- lot_ID (primary key)  
- contact_info_ID (foreign key)  
- capacity  

### **Contact_Info table**
- contact_info_ID (primary key)  
- contact_type  
- contact_value  

### **User table**
- user_ID (primary key)  
- contact_info_ID (foreign key)  

### **Vehicle table**
- vehicle_ID (primary key)  
- user_ID (foreign key)  
- vehicle_type  

### **Transaction table**
- transaction_ID (primary key)  
- user_ID (foreign key)  
- vehicle_ID (foreign key)  
- check_in_date_time  
- check_out_date_time  

Here "transaction" means a vehicle check-in and check-out.

---

## **High-level design**

- API Gateway handles TLS, routing, and DDoS protection.  
- Primary data stored in RDB.  
- Redis Cache stores frequently used data.  
- Reservation Service handles capacity checking & reservations.  
- Transaction Service handles arrivals & departures.  
- Payment System handles monetary operations.  
- Transaction Monitor checks for no-shows and charges 24 hours if needed.
<img width="1238" height="698" alt="image" src="https://github.com/user-attachments/assets/002955b7-6450-4322-865a-6d53d5ca59ec" />


Flow:
- reserve_spot(), vehicle_arrived(), vehicle_left(), etc.  

---

## **Request flows**

[Mid-level deep dive topic.]

1. **User calls check_capacity()**  
   → handled by Reservation Service, reads DB.

2. **User calls reserve_spot()**  
   → new entry added into Reservation table.  
   → returns payment URL or token requirement.

   Failure cases:  
   - Spot already taken (race condition) → return error  
   - Parking lot full → return error and maybe suggestions  

3. **User pays externally → receives token**  
   → calls complete_reservation()

4. **vehicle_arrived() / vehicle_left()**  
   → handled by Transaction Service (updates Transaction table)

5. **Transaction Monitor**  
   - Detects no-shows  
   - Cancels reservation after 8 hours past start  
   - Charges 1-day fee  

6. **Inconsistency handling**  
   - If vehicle_left() happens before vehicle_arrived()  
   - Notify admin  
   - Assume arrival at reservation_start_time  

---

## **Detailed component design**

[Senior level deep dive topic.]

### **Reservation Service**

Needs to find open spots efficiently.

### **Bitmap Approach**
- Divide day into 15-minute slots → 96 slots per day  
- 1 bit per slot → 96 bits per day  
- One year = ~4KB per spot  
- For 10 countries * 100 lots * 200 spots → ~8GB total  

Steps when calling reserve_spot():

1. Convert start_time & end_time into slot indices  
2. Create bitmap with 1’s for occupied range  
3. Compare bitmap with each spot’s occupancy bitmap  
4. Pick nearest open spot  
5. Write reservation object (protected by SELECT FOR UPDATE row lock)

Advantages:
- O(1) search per spot  
- Very fast, fits in RAM  

Disadvantages:
- 15-minute granularity  
- Memory required

### **Interval Tree Approach**
- Balanced BST storing (start, end)  
- O(log n) lookup  
- More memory efficient than bitmap  
- Slower

### **Which to choose?**
Bitmap is feasible:  
- ~8GB fits easily in cache + RDB  
- Super fast lookups  
- Good for interview scenario  

---

## **Trade offs / Tech choices**

RDB vs NoSQL:

- RDB gives strong consistency → avoids double booking  
- Data size (~300GB in 2 years) fits RDB comfortably  
- NoSQL gives better scalability but worse consistency guarantees  

Conclusion: **Use RDB**.

---

## **Failure scenarios / bottlenecks**

### **Scalability bottleneck**
Database will eventually be bottleneck.  
Solution: **partition by lot_ID**.

Benefits:
- All data for a parking lot (Reservation, Spot, Transactions) stays together  
- Avoids scatter-gather queries  
- Add read replicas  

### **Durability issues**

Large events (e.g., Olympics) → huge demand spike on one parking lot.

Bitmap approach + row locking ensures correctness because:
- Spot count is small (hundreds)
- Locked operations are short

### **Fault tolerance**
- Replicate DB across zones/regions  
- Monitor all servers and components  
- Stateless services scale horizontally  

---

## **Future improvements**
- Support different vehicle types, parking types  
- Additional services (charging stations, car wash, etc.)  
- Geographic optimizations  
  - Global Load Balancer  
  - Cross-region DB replication  

---
