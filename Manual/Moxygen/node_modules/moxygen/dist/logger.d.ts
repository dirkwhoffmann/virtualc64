export type LogLevel = 'error' | 'warn' | 'info' | 'verbose' | 'debug';
export declare const log: {
    error(message: string): void;
    warn(message: string): void;
    info(message: string): void;
    verbose(message: string): void;
    debug(message: string): void;
    setLevel(level: LogLevel): void;
    init(options: {
        quiet?: boolean;
        logfile?: string | boolean;
    }): void;
};
//# sourceMappingURL=logger.d.ts.map