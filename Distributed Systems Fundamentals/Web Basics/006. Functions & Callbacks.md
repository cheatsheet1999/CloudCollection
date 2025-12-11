## Why we need functions?
**The fact is, a lot of functions are not a good function  
Let's take a look at a example**

```js
function tenSquared() {
  return 10 * 10;
}

tenSquared();
```

**What about a 9 squared function? about 125 quared**

## Parameter - Don't repeat yourself
We don't want to write functions like that, repeat is the worst thing in programing.  
What could we do instead?  

**We write a more generalizable code piece**

```js
function squareNum(num) {
  return num * num;
}

squareNum(10);
squareNum(5);
```

**We can use num to represent any actual number withou repeating, that is what the parameter is for**

## Generalizing functions
`parameters` means we don't need to decide what data to run our functionality on until we run the function
- Then provide an actual value `argument` when we run the function   

### Higher order functions follow this same principle
- We may not want to decide exactly what some of our funcionality is until we run our function

## Repeating Functionality
<img width="481" alt="Screen Shot 2021-12-28 at 16 43 08" src="https://user-images.githubusercontent.com/37787994/147615098-c6669be3-c9f7-4e21-b9a9-69e9eecfcaf0.png">

<img width="592" alt="Screen Shot 2021-12-28 at 16 43 35" src="https://user-images.githubusercontent.com/37787994/147615117-66accb2d-487e-498c-9efa-4d758e7375a6.png">

### We could generalize our function, so we pass in our specific instruction only when we run `copyArrayAndManipulate`

```js
// This is our higher-order function
function copyArrayAndManipulate(array, instructions) {
  const output = [];
  for(let i = 0; i < array.length; i++) {
    output.push(instructions(array[i]));
  }
  return output;
}

// This is callback function
function multiplyBy2(input) {
  return input * 2;
}

const result = copyArrayAndManipulate([1,2,3], multiplyBy2);
```

