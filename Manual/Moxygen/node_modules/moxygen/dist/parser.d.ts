import type { Compound, MoxygenOptions, References } from './types.js';
/**
 * Load and parse Doxygen XML index.
 */
export declare function loadIndex(options: MoxygenOptions): Promise<{
    root: Compound;
    references: References;
}>;
//# sourceMappingURL=parser.d.ts.map