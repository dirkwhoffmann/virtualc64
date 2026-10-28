
---

{{cleanAnchor refid name}}

{{headingMarker 3}} {{name}}

{{badges}}

```java
{{signature}}
```

{{#if (linkedType)}}Type: {{linkedType}}

{{/if}}{{#if (sourceLabel)}}{{#if (sourceHref)}}[Defined in {{sourceLabel}}]({{sourceHref}}){{else}}Defined in {{sourceLabel}}{{/if}}
{{/if}}

{{briefdescription}}

{{detaileddescription}}

{{#if referencedBy}}
{{headingMarker 4}} Referenced by

{{#each referencedBy}}- {{linkedRelation this}}
{{/each}}

{{/if}}
{{#if references}}
{{headingMarker 4}} References

{{#each references}}- {{linkedRelation this}}
{{/each}}

{{/if}}
{{#if reimplements}}
{{headingMarker 4}} Reimplements

{{#each reimplements}}- {{linkedRelation this}}
{{/each}}

{{/if}}
{{#if reimplementedBy}}
{{headingMarker 4}} Reimplemented by

{{#each reimplementedBy}}- {{linkedRelation this}}
{{/each}}

{{/if}}

{{#unless briefdescription}}
{{#unless detaileddescription}}
{{memberSummary this}}
{{/unless}}
{{/unless}}

{{#if (hasDocumentedParams params)}}
{{headingMarker 4}} Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
{{#each (documentedParams params)}}| `{{name}}` | {{typeCell type}} | {{description}} |
{{/each}}
{{/if}}

{{#if returnValues}}
{{headingMarker 4}} Return Values

| Value | Description |
|-------|-------------|
{{#each returnValues}}| `{{name}}` | {{description}} |
{{/each}}
{{/if}}

{{#if exceptions}}
{{headingMarker 4}} Exceptions

| Exception | Description |
|-----------|-------------|
{{#each exceptions}}| `{{name}}` | {{description}} |
{{/each}}
{{/if}}

{{#if enumvalue}}
| Value | Description |
|-------|-------------|
{{#each enumvalue}}| `{{name}}` | {{summary}} |
{{/each}}
{{/if}}
