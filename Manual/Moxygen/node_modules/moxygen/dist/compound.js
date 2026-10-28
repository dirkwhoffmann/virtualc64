import { sectionLabel } from './vocabulary.js';
export function createCompound(parent = null, id = '', name = '') {
    return {
        parent,
        id,
        name,
        kind: '',
        refid: '',
        fullname: '',
        shortname: '',
        compounds: {},
        members: [],
        basecompoundref: [],
        derivedcompoundref: [],
        inheritedMemberGroups: [],
        allMembers: [],
        filtered: { compounds: [], members: [], sections: [] },
        briefdescription: '',
        detaileddescription: '',
        summary: '',
        namespace: '',
        templateParams: [],
    };
}
export function findCompound(root, id, name, create) {
    let compound = root.compounds[id];
    if (!compound && create) {
        compound = createCompound(root, id, name);
        root.compounds[id] = compound;
    }
    return compound;
}
/**
 * Recursively collect compounds (and optionally filter by kind) into a flat array.
 */
export function toArray(compound, type = 'compounds', kind) {
    if (type === 'members') {
        return [...compound.members];
    }
    const entries = Object.values(compound.compounds);
    const result = [];
    for (const child of entries) {
        if (!kind || child.kind === kind) {
            result.push(child);
            result.push(...toArray(child, type, kind));
        }
    }
    return result;
}
/**
 * Recursively collect filtered compounds into a flat array.
 */
export function toFilteredArray(compound, type = 'compounds') {
    const items = (type === 'compounds'
        ? compound.filtered.compounds
        : []);
    const result = [];
    for (const item of items) {
        result.push(item);
        result.push(...toFilteredArray(item, type));
    }
    return result;
}
/**
 * Filter a collection by a key matching allowed categories, optionally scoped to a group.
 */
export function filterCollection(collection, key, allowedCategories, groupid) {
    const categories = {};
    const items = Array.isArray(collection)
        ? collection
        : Object.values(collection);
    for (const item of items) {
        if (!item)
            continue;
        // Skip empty namespaces
        if ('filtered' in item &&
            item.kind === 'namespace') {
            const c = item;
            if ((!c.filtered.compounds || c.filtered.compounds.length === 0) &&
                (!c.filtered.members || c.filtered.members.length === 0)) {
                continue;
            }
        }
        // Skip items not belonging to current group. Nested groups own their
        // own groupid, but still belong in the parent group's topic list.
        if (groupid) {
            if ('filtered' in item) {
                const compound = item;
                const parent = compound.parent;
                if (compound.groupid !== groupid && !(compound.kind === 'group' && parent?.id === groupid)) {
                    continue;
                }
            }
            else if (item.groupid !== groupid) {
                continue;
            }
        }
        const categoryKey = item[key];
        if (!categories[categoryKey]) {
            categories[categoryKey] = [];
        }
        categories[categoryKey].push(item);
    }
    const result = [];
    for (const category of allowedCategories) {
        if (categories[category]) {
            result.push(...categories[category]);
        }
    }
    return result;
}
/**
 * Apply filters recursively to a compound and all its children.
 */
export function filterChildren(compound, filters, groupid) {
    const allCompounds = toArray(compound, 'compounds');
    for (const child of allCompounds) {
        child.filtered.members = filterCollection(child.members, 'section', filters.members, groupid);
        child.filtered.compounds = filterCollection(child.compounds, 'kind', filters.compounds, groupid);
    }
    compound.filtered.members = filterCollection(compound.members, 'section', filters.members, groupid);
    compound.filtered.compounds = filterCollection(compound.compounds, 'kind', filters.compounds, groupid);
}
const NOISE_RE = /^(TYPE|BREAK|DEG|SEP|IMPL)_\d+$/;
/**
 * Remove members that are structurally noise rather than API: internal macros
 * left behind by expansion.
 *
 * Missing documentation is deliberately not a signal. Doxygen only emits a
 * member when the project asked for it, either by documenting it, by placing
 * it in a documented group, or by setting `EXTRACT_ALL`. Include guards are
 * dropped by Doxygen itself and never reach this point.
 */
export function filterNoise(members) {
    return members.filter((m) => !NOISE_RE.test(m.name));
}
/**
 * Group filtered members by their section kind for structured output.
 */
export function groupMembersBySection(compound) {
    const groups = {};
    const order = [];
    for (const member of compound.filtered.members) {
        const key = member.section || 'func';
        if (!groups[key]) {
            groups[key] = [];
            order.push(key);
        }
        groups[key].push(member);
    }
    return order.map((section) => ({
        section,
        label: sectionLabel(section),
        members: groups[section],
    }));
}
//# sourceMappingURL=compound.js.map