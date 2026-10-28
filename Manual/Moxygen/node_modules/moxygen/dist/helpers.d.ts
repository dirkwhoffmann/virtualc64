import type { Compound, Member, MoxygenOptions, References } from './types.js';
export type AnchorMap = Map<string, string>;
export type PagePathMap = Map<string, string>;
/** The page and anchor a reference's target is drawn at. */
export interface LinkTarget {
    path: string;
    anchor: string;
}
/** The page being resolved, and where every drawn target landed. */
export interface PageLinks {
    path: string;
    targets: Map<string, LinkTarget>;
}
/**
 * Wrap code segments in backticks, preserving markdown links and line breaks.
 */
export declare function inline(code: string | string[]): string;
/**
 * Strip markdown links from generated type/signature text while preserving labels.
 */
export declare function stripMarkdownLinks(text: string): string;
/**
 * Convert a C++ symbol into a filesystem- and URL-safe path segment.
 */
export declare function safePathSegment(name: string): string;
/**
 * Generate an anchor string based on options.
 */
export declare function getAnchor(name: string, options: Pick<MoxygenOptions, 'anchors' | 'htmlAnchors'>): string;
/**
 * Find the nearest parent compound matching one of the given kinds.
 */
export declare function findParent(compound: Compound | Member | undefined, kinds: string[]): Compound | undefined;
/**
 * Convert a name to a clean URL-safe anchor ID.
 */
export declare function cleanId(name: string): string;
/**
 * Build a map from Doxygen refids to clean, human-readable anchor IDs.
 * Handles deduplication for overloaded names.
 */
export declare function buildCleanAnchorMap(compounds: Compound[]): AnchorMap;
/**
 * Resolve internal reference links to point to correct output files.
 */
export type SlugMap = Map<string, string>;
export declare function resolveRefs(content: string, compound: Compound, references: References, options: MoxygenOptions, anchorMap?: AnchorMap, slugMap?: SlugMap, pagePathMap?: PagePathMap, links?: PageLinks): string;
/**
 * Calculate the output file path for a compound.
 */
export declare function compoundPath(compound: Compound, options: MoxygenOptions): string;
/**
 * Render a compound's contents to a string, resolving refs.
 */
export declare function renderCompound(compound: Compound, contents: (string | undefined)[], references: References, options: MoxygenOptions, anchorMap?: AnchorMap, slugMap?: SlugMap, pagePathMap?: PagePathMap, links?: PageLinks): string;
/**
 * Write a compound's rendered contents to file, resolving refs first.
 */
export declare function writeCompound(compound: Compound, contents: (string | undefined)[], references: References, options: MoxygenOptions, anchorMap?: AnchorMap, pagePathMap?: PagePathMap, links?: PageLinks): void;
/**
 * Write content array to a file.
 */
export declare function writeFile(filepath: string, contents: string[]): void;
//# sourceMappingURL=helpers.d.ts.map