## Overview
There's no simple explanation for this; it is one of the most confusing concepts in JavaScript. A hand-wavey explanation is that the value of this depends on how the function is called. The following rules are applied:

1. If the new keyword is used when calling the function, this inside the function is a brand new object.
2. If apply, call, or bind are used to call/create a function, this inside the function is the object that is passed in as the argument.
3. If a function is called as a method, such as obj.method() — this is the object that the function is a property of.
4. If a function is invoked as a free function invocation, meaning it was invoked without any of the conditions present above, this is the global object. In a browser, it is the window object. If in strict mode ('use strict'), this will be undefined instead of the global object.
5. If multiple of the above rules apply, the rule that is higher wins and will set the this value.
6. If the function is an ES2015 arrow function, it ignores all the rules above and receives the this value of its surrounding scope at the time it is created.


### 1. Regular function

*`this` is pointing to Window*

```Javascript
function fn() {
    console.log('This is a function');
}

function fn() {
    console.log('This is a function' + this);
}

fn();
fn.call();


```

### 2. Method on objects

*`this` is pointing to object => o*

*because `o` called `sayHI` method*

```Javascript
let o = {
    sayHi: function() {
       console.log('This is a function');
    }
}

o.sayHi();
```

### 3. Constructor

*`this` is pointing to `rick` because `rick` instantiate the constructor*

*`this` in the prototype object is also pointing to `rick`*

```Javascript
function fn() {}
new fn();

let rick = new fn();
fn.prototype.start = function(){};
```

### 4. Register Event function

*`this` in register event function is pointing to the attached element, but in this example*

```Javascript
let btn = document.querySelector('button');
btn.onclick = function() {
	console.log(this)
}; //click the button to call the function
```

### 5. Timer function

*`this` is pointing to Window*

```Javascript
setInterval(function() {}, 1000); //execute function in every second
```

### 6. **IIFE** (Immediately Invoked Function Expression)

*`this` is pointing to Window*

```Javascript
(function() {
  console.log('This is a function')
})();
```


### Understanding of 'this' keyword
#### Scenario 1: 'this' in global environment
It is relatively straightforward in this scenario, the browser in the global environment invokes the function. 'this' is pointing to ** Window**. However, 'this' will point to **undefined** if 'use strict'.  

```JS
function f1 () {
    console.log(this)
}
function f2 () {
    'use strict'
    console.log(this)
}
f1() // window
f2() // undefined
```

There is a very similar question (tedious, useless, but interesting)

```JS
const foo = {
    bar: 10,
    fn: function() {
       console.log(this)
       console.log(this.bar)
    }
}
var fn1 = foo.fn
fn1()
```
**Output:**
```
window
undefined
```


- `this` is still pointing to the Window. Although function `fn` is invoked by the object `foo` (foo.fn), after assignment to fn1, there is a **()**.  That means we execute fn1 in the global environment. Therefore, the code above still outputs _window_ and _undefined_.  

Now, if we change to   

```JS
const foo = {
    bar: 10,
    fn: function() {
       console.log(this)
       console.log(this.bar)
    }
}
foo.fn() 
```

**Output**  
```
{bar: 10, fn: ƒ}
10
```

- `this` is pointing to the object that invokes the function. Whenever a preceding dot calls a function, the object before that dot is this.
- To be more clear, in `foo.fn()`, `this` is pointing to `foo`. **When executing function, if `this` in the function is invoked by parent scope, then `this` will point to parent scope environment, otherwise, `this` point to global environment**




#### Scenario 2: 'this' in the context environment
Let's dive into examples.  

```JS
const person = {
    name: 'Lucas',
    brother: {
        name: 'Mike',
        fn: function() {
            return this.name
        }
    }
}
console.log(person.brother.fn())
```

```
Mike
```

Why? Because in these nested relationships, `this` will point to the LAST object that invokes the function. Here is the `brother` scope, so we choose the `name` variable that is in `brother` scope.

Example 2:   

```JS
const o1 = {
    text: 'o1',
    fn: function() {
        return this.text
    }
}
const o2 = {
    text: 'o2',
    fn: function() {
        return o1.fn()
    }
}
const o3 = {
    text: 'o3',
    fn: function() {
        var fn = o1.fn
        return fn()
    }
}

console.log(o1.fn())
console.log(o2.fn())
console.log(o3.fn())
```

```
o1
o1
undefined
```

WHAT???
Let's dive into it: 
- First, we got `o1` because of `o1.fn()`, `o1` called `fn()` so this.text will print the variable that is inside `o1` object.
- Second, although it is the `o2` that invoked `fn()`, inside the function of o2, it is `o1.fn()`. Surprisingly, we jump to o1 and get `o1` as its input
- Last, after the `var fn` is assigned, it is a normal `fn()` call, so here this points to the Window, and the answer is, of course, undefined.

A follow-up question, if we want `o2` as output for  
`console.log(o2.fn())`  
**without using call, apply, bind**  
What should we do?   

```JS
const o1 = {
    text: 'o1',
    fn: function() {
        return this.text
    }
}
const o2 = {
    text: 'o2',
    fn: o1.fn
}

console.log(o2.fn())
```

We don't execute `o1.fn`, instead, we only need the `this.text` inside `o1.fn` function. So in o2, we put `this.text` on `o2.fn`. Then execute it in the `console.log(o2.fn())`


#### Scenario 3: bind / call / apply 

```JS
const foo = {
    name: 'lucas',
    logName: function() {
        console.log(this.name)
    }
}
const bar = {
    name: 'mike'
}
console.log(foo.logName.call(bar))
```
The above code will output `mike`   

#### Scenario 4: this and constructor

```JS
function Foo() {
    this.bar = "Lucas"
}
const instance = new Foo()
console.log(instance.bar)
```

The output will be `Lucas`, but what happens behind the scene after `new` with constructor `Foo`?   
- Create a new Object
- `this` in constructor will point to the new object
- Adding attributes, methods for the object
- return a new object

**The above can be shown as code below**  
```JS
let obj  = {}
obj.__proto__ = Foo.prototype
Foo.call(obj)
```
_Here, we only simulate a naive version of `new`_

Note, if `return` has been mentioned in a constructor, there are two cases needed to be considered

**First**  

```js
function Foo(){
    this.user = "Lucas"
    const o = {}
    return o
}
const instance = new Foo()
console.log(instance.user)
```
It will output `undefined` because `instance` is an empty object `o` just returned

**Second**

```JS
function Foo(){
    this.user = "Lucas"
    return 1
}
const instance = new Foo()
console.log(instance.user)
```
It will output `Lucas`, which means `instance` is the object instantiating `this`

***Conclusion: If a value is explicitly returned in the constructor and an object is returned, then `this` points to the returned object; if the returned object is not an object, then `this` still points to the instance.***

```
1. During function precompilation, `this` points to `window`    
2. In the global scope, `this` points to `window`   
3. call/apply can change `this` pointing    
4. obj.fn() who calls the method `this` points to whom    
```

#### Scenario 5: New story, `this` in the arrow function
`this` for arrow functions does not apply to the above standard rules, but is determined according to the outer (function or global) context scope.

```JS
const foo = {  
    fn: function () {  
        setTimeout(function() {  
            console.log(this)
        })
    }  
}  
console.log(foo.fn())
```

In this question, `this` is in an anonymous function of `setTimeout()`, so `this` is pointing to `window` object. However, we can use arrow functions to make `this` points to object `foo`

```JS
const foo = {  
    fn: function () {  
        setTimeout(() => {  
            console.log(this)
        })
    }  
} 
console.log(foo.fn())
```

#### Scenario 6: Prority of `this`
We often call the binding of this through `call`, `apply`, `bind`, and `new` as explicit binding; the pointing of `this` determined according to the call relationship is called implicit binding.  
But which one has a higher priority? 

```js
function foo (a) {
    console.log(this.a)
}

const obj1 = {
    a: 1,
    foo: foo
}

const obj2 = {
    a: 2,
    foo: foo
}

obj1.foo.call(obj2)
obj2.foo.call(obj1)
```

Without `call` function, the output will be `1` and `2`.    
Now the output is `2` and `1`  
which means `call`, `apply` has a higher priority. 

```js
function foo (a) {
    this.a = a
}

const obj1 = {}

var bar = foo.bind(obj1)
bar(2)
console.log(obj1.a)
```
By using `bind`, `this` in `bar` has been binded on `obj1`. After executing `bar(2)`, `obj1.a = 2`. Which is, after `bar(2)`, `obj1` is `{a: 2} 


Turn to arrow function
```js
function foo() {
    return a => {
        console.log(this.a)
    };
}

const obj1 = {
    a: 2
}

const obj2 = {
    a: 3
}

const bar = foo.call(obj1)
console.log(bar.call(obj2))
```
The output will be 2. We first bind `this`  in `foo()` on `obj1`. The `this` of `bar` (reference arrow function) will also be bound to `obj1`, and the arrow function binding cannot be modified. Note here, foo() is not an arrow function, that's why we can use `call` function on foo.

If we refactor `foo` to arrow function completely
```js
var a = 123
const foo = () => a => {
    console.log(this.a)
}

const obj1 = {
    a: 2
}

const obj2 = {
    a: 3
}

var bar = foo.call(obj1)
console.log(bar.call(obj2))
```

It will output `123`.

### Reference
https://www.zhihu.com/question/353757734/answer/964557747
https://blog.kevinchisholm.com/javascript/context-object-literals/
