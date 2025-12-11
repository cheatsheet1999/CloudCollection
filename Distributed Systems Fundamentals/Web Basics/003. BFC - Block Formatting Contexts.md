### Normal Flow

In normal flow, elements are laid out from top to bottom according to their position in HTML. During this process, **inline elements** are arranged horizontally until the line is filled and then wraps, and **block** elements are rendered as a complete new line , unless otherwise specified, all elements default to the normal flow positioning, it can also be said that the **position of the element in the normal flow is determined by the position of the element in the HTML document**

### Float
In a floating layout, elements first appear in the position of the normal flow, and then shift as far to the left or right as possible according to the direction of the float, with an effect similar to text wrapping in typographic layout.

### Absolute positionin
In an absolute positioning layout, elements are detached from the normal flow as a whole, so absolutely positioned elements do not affect their sibling elements, and the specific position of the elements is determined by the coordinates of absolute positioning.

## BFC
Elements with BFC characteristics can be regarded as isolated independent containers. Elements in the container will not affect the layout of elements outside, and BFC has some characteristics that ordinary containers do not have.

The BFC feature is triggered whenever an element satisfies any of the following conditions:

- body root element
- float element: float value other than none
- Absolutely positioned elements: position (absolute, fixed)
- display is inline-block, table-cells, flex
- overflow value other than visible (hidden, auto, scroll)

### BFC Features and Applications
1. The bottom margin of the same BFC will collapse


```html
<head>
div{
    width: 100px;
    height: 100px;
    background: lightblue;
    margin: 100px;
}
</head>
<body>
    <div></div>
    <div></div>
</body>
```

<img width="715" alt="Screen Shot 2022-01-19 at 00 21 59" src="https://user-images.githubusercontent.com/37787994/150083206-c8685d0d-e492-40a9-936c-f2ad44e33ca8.png">

From the effect point of view, because the two div elements are under the _same BFC container_ (here refers to the body element), the bottom margin of the first div and the top margin of the second div overlap, so the distance between the two boxes Only 100px, not 200px.

If we want to avoid overlapping margins, we can put it in different BFC containers.

```html
.container {
    overflow: hidden;
}
p {
    width: 100px;
    height: 100px;
    background: lightblue;
    margin: 100px;
}

<div class="container">
    <p></p>
</div>
<div class="container">
    <p></p>
</div>
```

**At this time, the margins of the two boxes become 200px**
<img width="715" alt="Screen Shot 2022-01-19 at 00 27 33" src="https://user-images.githubusercontent.com/37787994/150083930-652c702f-2bae-466f-879b-06bbb0ccf91f.png">












