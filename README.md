```
const { MatchSet } = require('matchset');
const matchSet = new MatchSet();
console.log(matchSet.match(10001, 98));         // null
console.log(matchSet.match(10002n, 99n));       // { self: '10002', peer: '10001' }
console.log(matchSet.match('10003', '198'));    // null
console.log(matchSet.match(10004, '99'));       // { self: '10004', peer: '10003' }
```