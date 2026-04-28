#!/usr/bin/env bash
set -eu

label=""
image=""
map_file=""
model_wrapper_obj=""
dispatch_obj=""
ai_runtime_obj=""
iree_root=""
output=""
map_model_wrapper_needle="generated/librv_aios_models.a(ai_models.c.obj)"
map_dispatch_needle="generated/librv_aios_models.a(st_mnist_28.o)"
regeneration_command="pixi run -e rv32 report-ai-footprint"

while [ "$#" -gt 0 ]; do
  case "$1" in
    --label)
      label="$2"
      shift 2
      ;;
    --image)
      image="$2"
      shift 2
      ;;
    --map)
      map_file="$2"
      shift 2
      ;;
    --model-wrapper-obj)
      model_wrapper_obj="$2"
      shift 2
      ;;
    --dispatch-obj)
      dispatch_obj="$2"
      shift 2
      ;;
    --ai-runtime-obj)
      ai_runtime_obj="$2"
      shift 2
      ;;
    --iree-root)
      iree_root="$2"
      shift 2
      ;;
    --map-model-wrapper-needle)
      map_model_wrapper_needle="$2"
      shift 2
      ;;
    --map-dispatch-needle)
      map_dispatch_needle="$2"
      shift 2
      ;;
    --regeneration-command)
      regeneration_command="$2"
      shift 2
      ;;
    --output)
      output="$2"
      shift 2
      ;;
    *)
      echo "unknown argument: $1" >&2
      exit 2
      ;;
  esac
done

if [ -z "$label" ] ||
   [ -z "$image" ] ||
   [ -z "$map_file" ] ||
   [ -z "$model_wrapper_obj" ] ||
   [ -z "$dispatch_obj" ] ||
   [ -z "$ai_runtime_obj" ] ||
   [ -z "$iree_root" ] ||
   [ -z "$output" ]; then
  echo "missing required arguments" >&2
  exit 2
fi

require_file() {
  if [ ! -f "$1" ]; then
    echo "missing file: $1" >&2
    exit 1
  fi
}

require_file "$image"
require_file "$map_file"
require_file "$model_wrapper_obj"
require_file "$dispatch_obj"
require_file "$ai_runtime_obj"

if [ ! -d "$iree_root" ]; then
  echo "missing directory: $iree_root" >&2
  exit 1
fi

size_row() {
  local path="$1"
  llvm-size "$path" | awk 'NR == 2 { printf "%s %s %s %s", $1, $2, $3, $4 }'
}

aggregate_size() {
  local root="$1"
  if ! find "$root" -name '*.obj' -print -quit | grep -q .; then
    printf "0 0 0 0"
    return
  fi
  find "$root" -name '*.obj' -print0 |
    xargs -0 llvm-size |
    awk 'NR > 1 && $1 != "text" { text += $1; data += $2; bss += $3; dec += $4 }
         END { printf "%d %d %d %d", text, data, bss, dec }'
}

map_sum() {
  local needle="$1"
  awk -v needle="$needle" '
    function h2d(value,    i, c, n, p) {
      n = 0
      value = tolower(value)
      for (i = 1; i <= length(value); ++i) {
        c = substr(value, i, 1)
        p = index("0123456789abcdef", c) - 1
        if (p < 0) return 0
        n = (n * 16) + p
      }
      return n
    }
    index($0, needle) && $0 ~ /:\(/ {
      sum += h2d($3)
    }
    END { printf "%d", sum }
  ' "$map_file"
}

format_tuple_table_row() {
  local name="$1"
  local tuple="$2"
  set -- $tuple
  printf "| %s | %s | %s | %s | %s |\n" "$name" "$1" "$2" "$3" "$4"
}

mkdir -p "$(dirname "$output")"

image_total=$(llvm-size --format=sysv "$image" |
  awk '$1 == "Total" { print $2 }')
model_wrapper_size=$(size_row "$model_wrapper_obj")
dispatch_size=$(size_row "$dispatch_obj")
ai_runtime_size=$(size_row "$ai_runtime_obj")
iree_total_size=$(aggregate_size "$iree_root")
iree_base_size=$(aggregate_size "$iree_root/iree/base")
iree_vm_size=$(aggregate_size "$iree_root/iree/vm")
iree_hal_size=$(aggregate_size "$iree_root/iree/hal")
iree_modules_size=$(aggregate_size "$iree_root/iree/modules")
iree_io_size=$(aggregate_size "$iree_root/iree/io")
map_model_wrapper=$(map_sum "$map_model_wrapper_needle")
map_dispatch=$(map_sum "$map_dispatch_needle")
map_ai_runtime=$(map_sum "librv_aios_ai.a(ai_model_registry.c.obj)")
map_iree_total=$(map_sum "third_party/iree/runtime/src")
map_iree_base=$(map_sum "third_party/iree/runtime/src/iree/base")
map_iree_vm=$(map_sum "third_party/iree/runtime/src/iree/vm")
map_iree_hal=$(map_sum "third_party/iree/runtime/src/iree/hal")
map_iree_modules=$(map_sum "third_party/iree/runtime/src/iree/modules")
map_iree_io=$(map_sum "third_party/iree/runtime/src/iree/io")

{
  printf "# AI Footprint Attribution: %s\n\n" "$label"
  printf "Generated: %s\n\n" "$(date -u '+%Y-%m-%dT%H:%M:%SZ')"

  printf "## Inputs\n\n"
  printf -- "- Image: \`%s\`\n" "$image"
  printf -- "- Map file: \`%s\`\n" "$map_file"
  printf -- "- Model wrapper object: \`%s\`\n" "$model_wrapper_obj"
  printf -- "- Dispatch object: \`%s\`\n" "$dispatch_obj"
  printf -- "- AI runtime object: \`%s\`\n" "$ai_runtime_obj"
  printf -- "- IREE runtime object root: \`%s\`\n\n" "$iree_root"
  printf -- "- Model wrapper map needle: \`%s\`\n" "$map_model_wrapper_needle"
  printf -- "- Dispatch map needle: \`%s\`\n\n" "$map_dispatch_needle"

  printf "## Image Total\n\n"
  printf "| Artifact | Bytes |\n"
  printf "| --- | ---: |\n"
  printf "| %s | %s |\n\n" "$image" "$image_total"

  printf "## Linked Map Attribution\n\n"
  printf "These values are summed from linker map entries that made it into the final image.\n\n"
  printf "| Component | Bytes |\n"
  printf "| --- | ---: |\n"
  printf "| generated model wrapper | %s |\n" "$map_model_wrapper"
  printf "| generated dispatch object | %s |\n" "$map_dispatch"
  printf "| RRTOS AI registry runtime | %s |\n" "$map_ai_runtime"
  printf "| IREE runtime linked contribution | %s |\n\n" "$map_iree_total"

  printf "## Linked IREE Runtime Breakdown\n\n"
  printf "| Component | Bytes |\n"
  printf "| --- | ---: |\n"
  printf "| iree/base | %s |\n" "$map_iree_base"
  printf "| iree/vm | %s |\n" "$map_iree_vm"
  printf "| iree/hal | %s |\n" "$map_iree_hal"
  printf "| iree/modules | %s |\n" "$map_iree_modules"
  printf "| iree/io | %s |\n\n" "$map_iree_io"

  printf "## Object Attribution Upper Bound\n\n"
  printf "These values come from object files before final link garbage collection.\n\n"
  printf "| Component | text | data | bss | dec |\n"
  printf "| --- | ---: | ---: | ---: | ---: |\n"
  format_tuple_table_row "generated model wrapper" "$model_wrapper_size"
  format_tuple_table_row "generated dispatch object" "$dispatch_size"
  format_tuple_table_row "RRTOS AI registry runtime" "$ai_runtime_size"
  format_tuple_table_row "IREE runtime objects aggregate" "$iree_total_size"
  printf "\n"

  printf "## IREE Runtime Breakdown\n\n"
  printf "| Component | text | data | bss | dec |\n"
  printf "| --- | ---: | ---: | ---: | ---: |\n"
  format_tuple_table_row "iree/base" "$iree_base_size"
  format_tuple_table_row "iree/vm" "$iree_vm_size"
  format_tuple_table_row "iree/hal" "$iree_hal_size"
  format_tuple_table_row "iree/modules" "$iree_modules_size"
  format_tuple_table_row "iree/io" "$iree_io_size"
  printf "\n"

  printf "## Model Constant Symbols\n\n"
  printf "| Symbol | Bytes |\n"
  printf "| --- | ---: |\n"
  llvm-nm --print-size --size-sort --radix=d "$model_wrapper_obj" |
    awk '$4 ~ /__const$/ { printf "| `%s` | %d |\n", $4, $2 + 0 }'
  printf "\n"

  printf "## Largest Generated Wrapper Symbols\n\n"
  printf "| Symbol | Bytes |\n"
  printf "| --- | ---: |\n"
  llvm-nm --print-size --size-sort --radix=d "$model_wrapper_obj" |
    awk 'NF >= 4 { symbol = $4; size = $2 + 0; rows[++count] = "| `" symbol "` | " size " |" }
         END {
           start = count - 19;
           if (start < 1) start = 1;
           for (i = start; i <= count; ++i) print rows[i];
         }'
  printf "\n"

  printf "## Regeneration\n\n"
  printf "\`\`\`bash\n"
  printf "%s\n" "$regeneration_command"
  printf "\`\`\`\n"
} > "$output"
