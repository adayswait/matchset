```
const { MatchSet } = require('matchset');
const matchSet = new MatchSet();
console.log(matchSet.match(10001, 98, 10));     // null
console.log(matchSet.match(10002n, 199n, 100)); // null
console.log(matchSet.match('10003', '198', 2)); // { self: '10003', peer: '10002' }
console.log(matchSet.match(10004, '99', '10')); // { self: '10004', peer: '10001' }
```