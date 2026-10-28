{{headingMarker 1}} {{#if this.shortname}}{{this.shortname}}{{else}}{{shortname name}}{{/if}} {{cleanAnchor refid name}}

{{briefdescription}}

{{detaileddescription}}

{{#if filtered.members}}
{{headingMarker 2}} Contents

| Section |
|---------|
{{#each filtered.members}}| [`{{name}}`](#{{cleanId refid name}}) |
{{/each}}
{{/if}}
