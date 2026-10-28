#!/usr/bin/env node
import { Command } from 'commander';
import { readFileSync } from 'node:fs';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';
import { run } from './index.js';
const __dirname = dirname(fileURLToPath(import.meta.url));
const pkg = JSON.parse(readFileSync(join(__dirname, '..', 'package.json'), 'utf8'));
const program = new Command();
program
    .name('moxygen')
    .version(pkg.version)
    .description('Doxygen XML to Markdown converter')
    .argument('<directory>', 'Doxygen XML directory')
    .option('-o, --output <file>', 'output file, must contain "%s" when using groups or classes')
    .option('-g, --groups', 'output doxygen groups into separate files', false)
    .option('-c, --classes', 'output doxygen classes into separate files', false)
    .option('-p, --pages', 'output doxygen pages into separate files', false)
    .option('-n, --noindex', 'disable generation of the index', false)
    .option('-a, --anchors', 'add anchors to internal links', false)
    .option('-H, --html-anchors', 'add HTML anchors to internal links', false)
    .option('-l, --language <lang>', 'programming language', 'cpp')
    .option('-F, --flavour <name>', 'markdown dialect for admonitions: pandoc or github', 'pandoc')
    .option('--flavor <name>', 'alias for --flavour')
    .option('-t, --templates <dir>', 'custom templates directory')
    .option('-s, --source-root <dir>', 'source root used to resolve file-level group membership')
    .option('-f, --frontmatter', 'prepend YAML frontmatter to output files', false)
    .option('-L, --logfile [file]', 'output log messages to file (default: "moxygen.log")')
    .option('-I, --inline-groups', 'inline group members into the parent compound instead of generating separate files', false)
    .option('-q, --quiet', 'quiet mode', false)
    .action(async (directory, opts) => {
    try {
        await run({
            directory,
            output: opts.output,
            groups: opts.groups,
            classes: opts.classes,
            pages: opts.pages,
            noindex: opts.noindex,
            anchors: opts.anchors,
            htmlAnchors: opts.htmlAnchors,
            language: opts.language,
            flavour: (opts.flavor ?? opts.flavour),
            templates: opts.templates,
            sourceRoot: opts.sourceRoot,
            frontmatter: opts.frontmatter,
            logfile: opts.logfile,
            quiet: opts.quiet,
            inlineGroups: opts.inlineGroups,
        });
    }
    catch (err) {
        console.error(err instanceof Error ? err.message : err);
        process.exit(1);
    }
});
program.parse();
//# sourceMappingURL=cli.js.map