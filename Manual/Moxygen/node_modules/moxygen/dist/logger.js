import { appendFileSync, writeFileSync } from 'node:fs';
let currentLevel = 'info';
let quiet = false;
let logFile = null;
const levels = {
    error: 0,
    warn: 1,
    info: 2,
    verbose: 3,
    debug: 4,
};
function shouldLog(level) {
    return levels[level] <= levels[currentLevel];
}
function formatMessage(level, message) {
    return `${level}: ${message}`;
}
function writeLog(level, message) {
    const formatted = formatMessage(level, message);
    // Write to file (always, regardless of quiet)
    if (logFile) {
        appendFileSync(logFile, formatted + '\n', 'utf8');
    }
    // Write to console (unless quiet)
    if (!quiet) {
        if (level === 'error') {
            console.error(formatted);
        }
        else if (level === 'warn') {
            console.warn(formatted);
        }
        else {
            console.log(formatted);
        }
    }
}
export const log = {
    error(message) {
        if (shouldLog('error'))
            writeLog('error', message);
    },
    warn(message) {
        if (shouldLog('warn'))
            writeLog('warn', message);
    },
    info(message) {
        if (shouldLog('info'))
            writeLog('info', message);
    },
    verbose(message) {
        if (shouldLog('verbose'))
            writeLog('verbose', message);
    },
    debug(message) {
        if (shouldLog('debug'))
            writeLog('debug', message);
    },
    setLevel(level) {
        currentLevel = level;
    },
    init(options) {
        if (options.quiet) {
            quiet = true;
        }
        if (typeof options.logfile === 'string') {
            logFile = options.logfile;
        }
        else if (options.logfile === true) {
            logFile = 'moxygen.log';
        }
        // When logging to file or not quiet, enable verbose output
        if (logFile || !quiet) {
            currentLevel = 'verbose';
        }
        // Truncate logfile at start of run
        if (logFile) {
            writeFileSync(logFile, '', 'utf8');
        }
    },
};
//# sourceMappingURL=logger.js.map