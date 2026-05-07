# Static-Direct Remap Plan

Plan `miniresnet_first_layer_patch` rebases one verified static-direct arena region after a generated patch replaces the earlier command range.

| Metric | Bytes | KiB |
| --- | ---: | ---: |
| Original arena | 351104 | 342.875 |
| Remapped arena | 142336 | 139.000 |
| Saved | 208768 | 203.875 |

| Region | Allocation | Old | New | Bytes | Commands |
| --- | --- | ---: | ---: | ---: | --- |
| rebased | `transient_buffer_1` | 244608 | 0 | 106496 | [5, 13) |
| scratch | `transient_buffer` | 0 | 106496 | 35840 | [5, 17) |

Fail-closed rules:

- Only remap constants in the declared allocation value range.
- Only shrink bindings that exactly match the declared original window.
- Reject slices that fall between the original binding base and rebased region.
- Reject retained commands after a remap end unless they use an explicit scratch rule.
