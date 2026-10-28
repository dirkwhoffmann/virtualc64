import type { AnchorMap } from './helpers.js';
import type { Compound, MoxygenOptions } from './types.js';
export interface RenderContext {
    headingBase: number;
}
/**
 * Set the anchor map used by cleanAnchor/cleanId helpers.
 * Call before rendering a batch of compounds.
 */
export declare function setAnchorMap(map: AnchorMap | undefined): void;
/**
 * Hear of every anchor a render draws, by the refid it stands for. An enum's
 * values are drawn in its own section, so a reference to one lands there too.
 */
export declare function recordAnchors(recorder: ((refid: string, anchor: string) => void) | undefined): void;
/**
 * Register Handlebars helpers for template rendering.
 */
export declare function registerHelpers(options: Pick<MoxygenOptions, 'anchors' | 'htmlAnchors' | 'sourceUrl'>): void;
/**
 * Load all .md templates from the given directory.
 */
export declare function load(templateDirectory: string): void;
/**
 * Render a single compound using the appropriate template.
 */
export declare function render(compound: Compound, context?: RenderContext): string | undefined;
/**
 * Render an array of compounds.
 */
export declare function renderArray(compounds: Compound[], contextFor?: (compound: Compound, index: number) => RenderContext): (string | undefined)[];
//# sourceMappingURL=templates.d.ts.map