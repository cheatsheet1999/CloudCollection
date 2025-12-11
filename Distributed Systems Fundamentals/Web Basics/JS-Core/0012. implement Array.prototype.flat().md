There is already [Array.prototype.flat()](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/flat) in JavaScript (ES2019), which reduces the nesting of Array.

Could you manage to implement your own one?

Here is an example to illustrate

***
const arr = [1, [2], [3, [4]]];

flat(arr)
// [1, 2, 3, [4]]

flat(arr, 1)
// [1, 2, 3, [4]]

flat(arr, 2)
// [1, 2, 3, 4]
***


**Follow up**

**Are you able to solve it both recursively and iteratively?**

1. Recursively
```Javascript

/**
 * @param { Array } arr
 * @param { number } depth
 * @returns { Array }
 */
function flat(arr, depth = 1) {
  let res = [];
  arr.forEach((item) => {
    if(Array.isArray(item) && depth > 0) {
      res.push(...flat(item, depth - 1));
    } else {
      res.push(item);
    }
  })
  return res;
}

```

2. Iteratively
```Javascript
/**
 * @param {Array} arr
 * @param {number} depth
 */

function flat(arr, depth = 1) {
  const result = []
  const stack = [...arr.map(item => ([item, depth]))]
  
  while (stack.length > 0) {
    const [top, depth] = stack.pop()
    if (Array.isArray(top) && depth > 0) {
      stack.push(...top.map(item => ([item, depth - 1])))
    } else {
      result.push(top)
    }
  }
  
  return result.reverse()
}
```
