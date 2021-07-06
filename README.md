```
const { MatchSet } = require('matchset');
const matchSet = new MatchSet();
console.log(matchSet.match(10001, 198, 10));
console.log(matchSet.cancel(10001));
console.log(matchSet.match(10002n, 199n, 100));
console.log(matchSet.match('10003', '198', 2));
console.log(matchSet.match(10004, 200, 5));

for (let i = 0; i < 1000000; i++) {
    const p1 = Math.floor(Math.random() * 100000000);
    const p2 = Math.floor(Math.random() * 100000000);
    const p3 = Math.floor(Math.random() * 100000);
    console.log(i, p1, p2, p3, matchSet.match(p1, p2, p3))
    if (Math.random() > 0.5) {
        console.log(matchSet.cancel(p1));
    }
}
```