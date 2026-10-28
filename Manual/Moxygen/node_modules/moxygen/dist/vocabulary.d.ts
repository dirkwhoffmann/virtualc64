/**
 * The display vocabulary: how Doxygen's internal names are shown to a reader.
 *
 * Two axes exist and they do not collapse into one. Doxygen labels a member
 * with a `kind` (`function`, `define`) and files it under a `section`
 * (`public-func`, `define`), and the mapping between them is many-to-one, so
 * a single table cannot serve both. Keeping them adjacent is the point: the
 * wording drifted apart while these lived in three different modules.
 */
/** Section keys to the heading shown above that group of members. */
export declare const SECTION_LABELS: Record<string, string>;
export declare function sectionLabel(section: string): string;
export declare function sectionHasReturnColumn(section: string): boolean;
/** Member kinds to the grouping used by the generated search index. */
export declare function searchCategoryForKind(kind: string): string;
//# sourceMappingURL=vocabulary.d.ts.map