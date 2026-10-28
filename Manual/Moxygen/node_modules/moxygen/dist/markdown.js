export function refLink(text, refid) {
    return link(text, `{#ref ${refid} #}`);
}
export function link(text, href) {
    return `[${text}](${href})`;
}
export function safeLink(text, href) {
    const normalized = href.trim();
    return isSafeHref(normalized) ? link(text, normalized) : text;
}
export function isSafeHref(href) {
    if (!href)
        return false;
    if (href.startsWith('#') || href.startsWith('/') || href.startsWith('./') || href.startsWith('../')) {
        return true;
    }
    if (/^(?:https?|ftp):\/\//i.test(href) || /^mailto:/i.test(href)) {
        return true;
    }
    if (/^[a-z][a-z0-9+.-]*:/i.test(href)) {
        return false;
    }
    return true;
}
export const escape = {
    row(text) {
        return text.replace(/\s*\|\s*$/, '');
    },
    cell(text) {
        return text
            .replace(/^[\n]+|[\n]+$/g, '')
            .replace(/\|/g, '\\|')
            .replace(/\n/g, '<br/>');
    },
};
//# sourceMappingURL=markdown.js.map