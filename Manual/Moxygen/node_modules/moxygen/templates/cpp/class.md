{{cleanAnchor refid name}}

{{headingMarker 1}} {{shortname name}}

{{#if includes}}
```cpp
#include <{{includes}}>
```
{{/if}}

```cpp
{{classSignature}}
```

{{#if (sourceLabel)}}{{#if (sourceHref)}}[Defined in {{sourceLabel}}]({{sourceHref}}){{else}}Defined in {{sourceLabel}}{{/if}}
{{/if}}

{{#if basecompoundref}}> **Inherits:** {{#each basecompoundref}}{{linkedName name refid}}{{#unless @last}}, {{/unless}}{{/each}}
{{/if}}
{{#if derivedcompoundref}}> **Subclassed by:** {{#each derivedcompoundref}}{{linkedName name refid}}{{#unless @last}}, {{/unless}}{{/each}}
{{/if}}

{{briefdescription}}

{{detaileddescription}}

{{#if (hasInheritedMembers allMembers)}}
{{headingMarker 2}} List of all members

| Name | Kind | Owner |
|------|------|-------|
{{#each allMembers}}| {{linkedName name refid}} | `{{kind}}` | {{#if inherited}}Inherited from {{linkedName owner ownerRefid}}{{else}}Declared here{{/if}} |
{{/each}}

{{/if}}
{{#each inheritedMemberGroups}}
{{headingMarker 2}} Inherited from {{linkedName name refid}}

| Kind | Name | Description |
|------|------|-------------|
{{#each members}}| `{{kind}}` | {{linkedName name refid}} {{badges}} | {{cell (memberSummary this)}} |
{{/each}}

{{/each}}
{{#each filtered.sections}}
{{headingMarker 2}} {{label}}

{{#if (hasReturnColumn section)}}
| Return | Name | Description |
|--------|------|-------------|
{{#each members}}| {{returnTypeShort}} | [`{{name}}`](#{{cleanId refid name}}) {{badges}} | {{cell (memberSummary this)}} |
{{/each}}
{{else}}
| Name | Description |
|------|-------------|
{{#each members}}| [`{{name}}`](#{{cleanId refid name}}) {{badges}} | {{cell (memberSummary this)}} |
{{/each}}
{{/if}}

{{#each members}}
{{> member}}
{{/each}}
{{/each}}
