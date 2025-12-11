**A promise is an object that may produce a single value some time in the future: either a resolved value, or a reason that it’s not resolved (e.g., a network error occurred). A promise may be in one of 3 possible states: fulfilled, rejected, or pending. Promise users can attach callbacks to handle the fulfilled value or the reason for rejection**

## How Promises Work
**A promise is an object which can be returned synchronously from an asynchronous function. It will be in one of 3 possible states:**
- Fulfilled: onFulfilled() will be called (e.g., resolve() was called)
- Rejected: onRejected() will be called (e.g., reject() was called)
- Pending: not yet fulfilled or rejected

**Promises following the spec must follow a specific set of rules:**
- A promise or “thenable” is an object that supplies a standard-compliant .then() method.
- A pending promise may transition into a fulfilled or rejected state.
- A fulfilled or rejected promise is settled, and must not transition into any other state.
- Once a promise is settled, it must have a value (which may be undefined). That value must not change.

## Basic promise
```js
const p1 = new Promise((resolve, reject) => {
  console.log('create a promise');
  resolve('Successed');
})

console.log("after new promise");

const p2 = p1.then(data => {
  console.log(data)
  throw new Error('Failed')
})

const p3 = p2.then(data => {
  console.log('success', data)
}, err => {
  console.log('faild', err)
})

```


## Syncronous Promise
```js
// Three status：PENDING、FULFILLED、REJECTED
const PENDING = 'PENDING';
const FULFILLED = 'FULFILLED';
const REJECTED = 'REJECTED';

class Promise {
  constructor(executor) {
    //Default status: PENDING
    this.status = PENDING;
    // Placeholder for success: undefined
    this.value = undefined;
    // Placeholder for failure undefined
    this.reason = undefined;

    // success when calling this function
    let resolve = (value) => {
       // The status can only be updated when the status is PENDING, preventing the resovle/reject method from being called twice in the executor
      if(this.status ===  PENDING) {
        this.status = FULFILLED;
        this.value = value;
      }
    } 

    // faila when calling this function
    let reject = (reason) => {
     // The status can only be updated when the status is PENDING, preventing the resovle/reject method from being called twice in the executor
      if(this.status ===  PENDING) {
        this.status = REJECTED;
        this.reason = reason;
      }
    }

    try {
      // Execute immediately, passing the resolve and reject functions to the consumer
      executor(resolve,reject)
    } catch (error) {
      // executing failure reason
      reject(error)
    }
  }

  // Contains a then method and receives two arguments, onFilled, onRejected
  then(onFulfilled, onRejected) {
    if (this.status === FULFILLED) {
      onFulfilled(this.value)
    }

    if (this.status === REJECTED) {
      onRejected(this.reason)
    }
  }
}

```

## Promise

```js
const PENDING = 'PENDING';
const FULFILLED = 'FULFILLED';
const REJECTED = 'REJECTED';

class Promise {
  constructor(executor) {
    this.status = PENDING;
    this.value = undefined;
    this.reason = undefined;
    // store callbacks when success
    this.onResolvedCallbacks = [];
    // store callbacks when fail
    this.onRejectedCallbacks= [];

    let resolve = (value) => {
      if(this.status ===  PENDING) {
        this.status = FULFILLED;
        this.value = value;
        // executing corresponding function
        this.onResolvedCallbacks.forEach(fn=>fn());
      }
    } 

    let reject = (reason) => {
      if(this.status ===  PENDING) {
        this.status = REJECTED;
        this.reason = reason;
        // executing corresponding function
        this.onRejectedCallbacks.forEach(fn=>fn());
      }
    }

    try {
      executor(resolve,reject)
    } catch (error) {
      reject(error)
    }
  }

  then(onFulfilled, onRejected) {
    if (this.status === FULFILLED) {
      onFulfilled(this.value)
    }

    if (this.status === REJECTED) {
      onRejected(this.reason)
    }

    if (this.status === PENDING) {
      // If the state of the promise is pending, we need to store the onFulfilled and onRejected functions, wait for the status to be determined, and then execute the corresponding functions in turn

      this.onResolvedCallbacks.push(() => {
        onFulfilled(this.value)
      });

      // If the state of the promise is pending, you need to store the onFulfilled and onRejected functions, wait for the status to be determined, and then execute the corresponding functions in turn

      this.onRejectedCallbacks.push(()=> {
        onRejected(this.reason);
      })
    }
  }
}

```
