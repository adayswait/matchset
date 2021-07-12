```
const { MatchSet } = require('matchset');
const matchSet = new MatchSet();

console.log(matchSet.match(10001, 200, 10, ["ip:'10.1.1.248'", "uid:10001"], ["ip:'10.1.1.248'", "uid:10002"]));
console.log(matchSet.match(10002, 198, 10, ["ip:'10.1.1.133'", "uid:10002"], ["ip:'10.1.1.133'"]));
console.log(matchSet.match(10003, 198, 10, ["ip:'10.1.1.143'", "uid:10003"], ["ip:'10.1.1.143'", "ip:'10.1.1.133'"]));

// console.log(matchSet.match(10001, 200, 10, ['a'], []));
// console.log(matchSet.match(10002, 198, 10, [], ['a']));
// console.log(matchSet.match(10003, 198, 10, [], []));

// for (let i = 0; i < 1000000; i++) {
//     const p1 = Math.floor(Math.random() * 100000000);
//     const p2 = Math.floor(Math.random() * 100000000);
//     const p3 = Math.floor(Math.random() * 100000);
//     console.log(i, p1, p2, p3, matchSet.match(p1, p2, p3, ["a", "c", "e"], ["b", "f"]))
//     if (Math.random() > 0.5) {
//         console.log(matchSet.cancel(p1));
//     }
// }
```