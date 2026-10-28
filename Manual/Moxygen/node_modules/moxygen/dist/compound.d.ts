import type { Compound, Filters, Member, SectionGroup } from './types.js';
export declare function createCompound(parent?: Compound | null, id?: string, name?: string): Compound;
export declare function findCompound(root: Compound, id: string, name: string, create: boolean): Compound | undefined;
/**
 * Recursively collect compounds (and optionally filter by kind) into a flat array.
 */
export declare function toArray(compound: Compound, type?: 'compounds' | 'members', kind?: string): (Compound | Member)[];
/**
 * Recursively collect filtered compounds into a flat array.
 */
export declare function toFilteredArray(compound: Compound, type?: 'compounds' | 'members'): Compound[];
/**
 * Filter a collection by a key matching allowed categories, optionally scoped to a group.
 */
export declare function filterCollection(collection: Record<string, Compound | Member> | (Compound | Member)[], key: string, allowedCategories: string[], groupid?: string): (Compound | Member)[];
/**
 * Apply filters recursively to a compound and all its children.
 */
export declare function filterChildren(compound: Compound, filters: Filters, groupid?: string): void;
/**
 * Remove members that are structurally noise rather than API: internal macros
 * left behind by expansion.
 *
 * Missing documentation is deliberately not a signal. Doxygen only emits a
 * member when the project asked for it, either by documenting it, by placing
 * it in a documented group, or by setting `EXTRACT_ALL`. Include guards are
 * dropped by Doxygen itself and never reach this point.
 */
export declare function filterNoise(members: Member[]): Member[];
/**
 * Group filtered members by their section kind for structured output.
 */
export declare function groupMembersBySection(compound: Compound): SectionGroup[];
//# sourceMappingURL=compound.d.ts.map