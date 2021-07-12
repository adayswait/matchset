const { MatchSet } = require('bindings')('matchset');
class MatchSetWrapper extends MatchSet {
    constructor() {
        super();
    }
    match(uid, score, limit, attrs, bans) {
        if (!Array.isArray(attrs) || !Array.isArray(bans)) {
            throw new Error('Invalid Parameters');
        }
        for (let i = 0; i < attrs.length; i++) {
            attrs[i] = attrs[i].toString();
        }
        for (let i = 0; i < bans.length; i++) {
            bans[i] = bans[i].toString();
        }
        const ret = this.Match(
            MatchSetWrapper.transfer2string(uid),
            MatchSetWrapper.transfer2string(score),
            MatchSetWrapper.transfer2string(limit),
            attrs, bans);
        if (ret.peer == '0') {
            return null;
        } else {
            return ret;
        }
    }
    cancel(uid) {
        return this.Cancel(MatchSetWrapper.transfer2string(uid));
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
        throw new Error('Invalid Parameters');
    }
}

exports.MatchSet = MatchSetWrapper;
