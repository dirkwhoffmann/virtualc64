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
export const SECTION_LABELS = {
    'public-type': 'Public Types',
    'public-func': 'Public Methods',
    'public-static-func': 'Public Static Methods',
    'protected-func': 'Protected Methods',
    'private-func': 'Private Methods',
    'private-static-func': 'Private Static Methods',
    'public-attrib': 'Public Attributes',
    'public-static-attrib': 'Public Static Attributes',
    'protected-attrib': 'Protected Attributes',
    'private-attrib': 'Private Attributes',
    'private-static-attrib': 'Private Static Attributes',
    'signal': 'Signals',
    'public-slot': 'Public Slots',
    'protected-slot': 'Protected Slots',
    'private-slot': 'Private Slots',
    'property': 'Properties',
    'enum': 'Enumerations',
    'typedef': 'Typedefs',
    'friend': 'Friends',
    'define': 'Macros',
    'func': 'Functions',
    'var': 'Variables',
};
/** Sections whose members have no return type worth a column of its own. */
const SECTIONS_WITHOUT_RETURN = new Set(['enum', 'define', 'public-type', 'friend']);
export function sectionLabel(section) {
    return SECTION_LABELS[section] || section;
}
export function sectionHasReturnColumn(section) {
    return !SECTIONS_WITHOUT_RETURN.has(section);
}
/** Member kinds to the grouping used by the generated search index. */
export function searchCategoryForKind(kind) {
    switch (kind) {
        case 'function':
        case 'signal':
        case 'slot':
            return 'Functions';
        case 'typedef':
            return 'Types';
        case 'enum':
            return 'Enums';
        case 'variable':
            return 'Variables';
        case 'friend':
            return 'Friends';
        case 'property':
            return 'Properties';
        case 'define':
            return 'Macros';
        default:
            return 'Members';
    }
}
//# sourceMappingURL=vocabulary.js.map