declare module "matchset" {
    export class MatchSet {
        constructor();
        match(uid: number | bigint | string,
            score: number | bigint | string,
            limit: number | bigint | string)
            : null | { self: string, peer: string };

        cancel(uid: number | bigint | string): boolean;
    }
}
