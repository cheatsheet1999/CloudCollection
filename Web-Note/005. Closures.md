<img width="1000" alt="Screen Shot 2022-01-29 at 12 54 20" src="https://user-images.githubusercontent.com/37787994/151675604-19cdd410-2e6b-4b2f-9a50-da691db22e8e.png">

### The closure is a function that accesses its lexical scope even executed outside of its lexical scope
The closure is a function that have access to the outer function's variables by using scope chain even after the outer function has returned.

Simpler, the closure is a function that remembers the variables from the place where it is defined, regardless of where it is executed later.

A rule of thumb to identify a closure: if inside a function you see an alien variable (not defined inside that function), most likely that function is a closure because the alien variable is captured.


```js
function outer() {
  let counter = 0;
  function incrementCounter() {
    counter++;
  }
  return incrementCounter;
}

let myNewFunction = outer();
myNewFunction();
```

## Backpack

- Closed over 'Variable environment' (C.O.V.E)
- Persistant Lexical Scope Referenced Data (P.L.S.R.D)
The backpack (or closure) of live data is attached incrementCounter(then to myNewFunction) through a hiddent property known as [[scope]] which persists when the inner function is returned out.

### Another example
<img width="802" alt="Screen Shot 2022-01-29 at 20 57 26" src="https://user-images.githubusercontent.com/37787994/151686055-e094031d-ae05-4975-999a-950e4cdf9edf.png">

**The output will be  
1 2 1 2**

Because myNewFunction and anotherFunction have two different backpacks

However, if initialize 0 inside incrementCounter, result will be 1 1 1 1   
because as long as the scope find the identifier, it will not go any further   
