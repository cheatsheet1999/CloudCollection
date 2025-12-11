
<img width="1233" alt="Screen Shot 2021-11-04 at 1 21 28 AM" src="https://user-images.githubusercontent.com/37787994/140280608-564a2bc3-c901-4444-b679-02ca6f90af73.png">

#### As the graph shows, after clicking the <a></a> element, Javascript will generate click event right away
- However, the event is actually not generated at the **target element** `<a></a>`. It's actually generated at the **top** of the DOM tree, which is the root of the DOCUMENT

#### It triggers the so called CAPTURING PHASE, where the event travels all the way down from the document root to the target element. It will pass down every single element, from Document to HTML - body- section -....- until finally reach the target.

#### As long as the event reaches the target, the target phase begins

A common example is the use of event listener

```JS
document.querySelector('a).addEventListener('click', () => {
alert('You clicked me')
})
```
Event listener wait for a certain event to happen on a certain element, and as soon as the event occurs, it runs the attached callback function. Again, this happens in the **target phase**

#### Bubbling phase
- After reaching the Target Phase, the event travers all the way up again, so we say that the event **bubble up** from the target to the document route
- As the event travels down and up the tree, they pass through all the parent elements, but not through any sibling element


#### By default, events can only be handled in the target, and in the bubbling phase
- However, we can set up an event listener to events in the **capturing phase** instead

#### Not all types of events have a capturing and bubbling phase, some of them are created right on the target element. But most of event do handling capturing and bubble.
