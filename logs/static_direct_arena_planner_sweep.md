# Static-Direct Arena Planner Sweep

| Model | Allocs | Linear KiB | Current KiB | Best KiB | Best Planner | Save vs Current KiB | Save vs Linear KiB |
| --- | ---: | ---: | ---: | ---: | --- | ---: | ---: |
| `miniresnetv2_s1_64x50_tl_int8_fresh_hal_schedule` | 12 | 805.039 | 525.125 | 525.125 | `input_order_first_fit` | 0.000 | 279.914 |
| `mobilenetv2_a035_128_int8_fresh_hal_schedule` | 36 | 3891.969 | 1937.125 | 1937.125 | `input_order_first_fit` | 0.000 | 1954.844 |
| `resnet8_32_tfs_int8_fresh_hal_schedule` | 15 | 774.977 | 527.312 | 527.312 | `size_desc_best_fit` | 0.000 | 247.664 |
| `squeezenetv11_128_tfs_int8_fresh_hal_schedule` | 6 | 2229.520 | 2229.125 | 2229.125 | `input_order_first_fit` | 0.000 | 0.395 |
| `st_yololcv1_192_int8_fresh_hal_schedule` | 2 | 1653.875 | 1653.875 | 1653.875 | `input_order_first_fit` | 0.000 | 0.000 |
| `fastdepth_224_int8_fresh_hal_schedule` | 2 | 5439.875 | 5439.875 | 5439.875 | `input_order_first_fit` | 0.000 | 0.000 |
| `handlandmarks_full_224_int8_fresh_hal_schedule` | 2 | 2752.188 | 2752.188 | 2752.188 | `input_order_first_fit` | 0.000 | 0.000 |

Planner definitions:

- `linear_no_reuse`: place every transient allocation sequentially.
- `input_order_first_fit`: current generator behavior.
- `size_desc_first_fit`: first-fit after placing larger live ranges first.
- `start_size_first_fit`: first-fit by lifetime start, with larger ties first.
- `size_desc_best_fit`: larger live ranges first, choosing the candidate that minimizes arena growth.
