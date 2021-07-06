const { MatchSet } = require('bindings')('matchset');
class MatchSetWrapper extends MatchSet {
    constructor() {
        super();
    }
    match(uid, score, limit) {
        const ret = this.Match(
            MatchSetWrapper.transfer2string(uid),
            MatchSetWrapper.transfer2string(score),
            MatchSetWrapper.transfer2string(limit));
        if (ret.peer == '0') {
            return null;
        } else {
            return ret;
        }
    }
    static transfer2string(param) {
        if (typeof param === 'number') {
            return Number.parseInt(param).toString();
        }
        if (typeof param === "string") {
            return Number.parseInt(param).toString();
        }
        if (typeof param === "bigint") {
            return param.toString();
        }
        throw new Error('Invalid Parameter');
    }
}

exports.MatchSet = MatchSetWrapper;
