# Monolith — Testing Reference

Last updated: 2026-03-07

---

## Test Environment

- **Engine:** Unreal Engine 5.7
- **Project:** Leviathan (`D:\Unreal Projects\Leviathan\`)
- **Plugin location (dev):** `C:\Projects\Monolith\`
- **Plugin location (test):** `D:\Unreal Projects\Leviathan\Plugins\Monolith\`
- **MCP endpoint:** `http://localhost:9316/mcp`

---

## How to Test

### Build Test

```bash
'C:\Program Files (x86)\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe' LeviathanEditor Win64 Development '-Project=D:\Unreal Projects\Leviathan\Leviathan.uproject' -waitmutex
```

### MCP Connection Test

```python
# Python http.client test (use NEW connection per request)
import http.client, json

conn = http.client.HTTPConnection("localhost", 9316)
body = json.dumps({"jsonrpc":"2.0","id":1,"method":"tools/list","params":{}})
conn.request("POST", "/mcp", body, {"Content-Type": "application/json"})
resp = conn.getresponse()
print(json.loads(resp.read()))
conn.close()
```

### Action Test Template

```python
import http.client, json

def test_action(namespace, action, params=None):
    conn = http.client.HTTPConnection("localhost", 9316)
    if namespace == "monolith":
        tool_name = f"monolith_{action}"
        args = params or {}
    else:
        tool_name = f"{namespace}.query"
        args = {"action": action}
        if params:
            args["params"] = params

    body = json.dumps({
        "jsonrpc": "2.0", "id": 1,
        "method": "tools/call",
        "params": {"name": tool_name, "arguments": args}
    })
    conn.request("POST", "/mcp", body, {"Content-Type": "application/json"})
    resp = conn.getresponse()
    result = json.loads(resp.read())
    conn.close()
    return result

# Example:
# test_action("monolith", "status")
# test_action("blueprint", "list_graphs", {"asset_path": "/Game/MyBlueprint"})
```

---

## Test Matrix

### Legend
- PASS = Verified working
- FAIL = Broken, see notes
- SKIP = Not testable (stub/unimplemented)
- UNTESTED = Not yet verified

---

### MonolithCore (namespace: "monolith")

| Action | Status | Notes |
|--------|--------|-------|
| `discover` | UNTESTED | Test with and without namespace filter |
| `discover(namespace)` | UNTESTED | Test each namespace: blueprint, material, animation, niagara, editor, config, project, source |
| `status` | UNTESTED | Should return version, uptime, port, action count, engine_version, project_name |
| `update(check)` | UNTESTED | Requires internet connection to GitHub |
| `update(install)` | UNTESTED | Windows-only, requires internet |
| `reindex` | UNTESTED | Should trigger MonolithIndex full rebuild via reflection |

### MonolithBlueprint (namespace: "blueprint")

| Action | Status | Notes |
|--------|--------|-------|
| `list_graphs` | UNTESTED | Test with a Blueprint that has event graphs + function graphs + macros |
| `get_graph_data` | UNTESTED | Test pin serialization for various types (exec, object, struct, enum) |
| `get_variables` | UNTESTED | Test container types (array, set, map), replication flags |
| `get_execution_flow` | UNTESTED | Test branching (Branch node, Switch), deep chains, diamonds |
| `search_nodes` | UNTESTED | Test case-insensitivity, partial matches |

### MonolithMaterial (namespace: "material")

| Action | Status | Notes |
|--------|--------|-------|
| `get_all_expressions` | UNTESTED | Test on complex material with many expression types |
| `get_expression_details` | UNTESTED | Test TextureSample, Parameter, Custom, Comment, FunctionCall |
| `get_full_connection_graph` | UNTESTED | Test all wire types |
| `disconnect_expression` | UNTESTED | Test input disconnect vs output disconnect |
| `build_material_graph` | UNTESTED | Test full PBR setup from JSON spec |
| `begin_transaction` | UNTESTED | Test undo after transaction |
| `end_transaction` | UNTESTED | Test without begin (should error gracefully) |
| `export_material_graph` | UNTESTED | Test round-trip: export -> import -> compare |
| `import_material_graph` | UNTESTED | Test both "overwrite" and "merge" modes |
| `validate_material` | UNTESTED | Test with: island nodes, broken textures, duplicate params |
| `render_preview` | UNTESTED | Check PNG output in Saved/Monolith/previews/ |
| `get_thumbnail` | UNTESTED | Verify base64 PNG is valid |
| `create_custom_hlsl_node` | UNTESTED | Test with inputs, outputs, and HLSL code |
| `get_layer_info` | UNTESTED | Test with Material Layer and Material Layer Blend |

### MonolithAnimation (namespace: "animation")

| Action | Status | Notes |
|--------|--------|-------|
| `add_montage_section` | UNTESTED | |
| `delete_montage_section` | UNTESTED | Test boundary: delete last section |
| `set_section_next` | UNTESTED | Test circular linking |
| `set_section_time` | UNTESTED | |
| `add_blendspace_sample` | UNTESTED | |
| `edit_blendspace_sample` | UNTESTED | Tests delete+re-add workaround |
| `delete_blendspace_sample` | UNTESTED | Test boundary: delete only sample |
| `get_state_machines` | UNTESTED | Test ABP with multiple state machines |
| `get_state_info` | UNTESTED | |
| `get_transitions` | UNTESTED | Test with empty machine_name (all SMs) |
| `get_blend_nodes` | UNTESTED | |
| `get_linked_layers` | UNTESTED | |
| `get_graphs` | UNTESTED | |
| `get_nodes` | UNTESTED | Test with and without class filter |
| `set_notify_time` | UNTESTED | |
| `set_notify_duration` | UNTESTED | |
| `set_bone_track_keys` | UNTESTED | Test JSON array format [[x,y,z],...] |
| `add_bone_track` | UNTESTED | |
| `remove_bone_track` | UNTESTED | Fixed 2026-03-07: now uses RemoveBoneCurve per bone + child traversal |
| `add_virtual_bone` | UNTESTED | |
| `remove_virtual_bones` | UNTESTED | Test specific names vs remove all |
| `get_skeleton_info` | UNTESTED | |
| `get_skeletal_mesh_info` | UNTESTED | |

### MonolithNiagara (namespace: "niagara")

| Action | Status | Notes |
|--------|--------|-------|
| `add_emitter` | UNTESTED | UE 5.7 FGuid VersionGuid |
| `remove_emitter` | UNTESTED | |
| `duplicate_emitter` | UNTESTED | |
| `set_emitter_enabled` | UNTESTED | |
| `reorder_emitters` | UNTESTED | Fixed 2026-03-07: now uses PostEditChange + MarkPackageDirty for proper change notifications |
| `set_emitter_property` | UNTESTED | Test each: SimTarget, bLocalSpace, bDeterminism, etc. |
| `request_compile` | UNTESTED | |
| `create_system` | UNTESTED | Test blank + from template |
| `get_ordered_modules` | UNTESTED | Test each usage: system_spawn, system_update, particle_spawn, particle_update |
| `get_module_inputs` | UNTESTED | |
| `get_module_graph` | UNTESTED | |
| `add_module` | UNTESTED | |
| `remove_module` | UNTESTED | |
| `move_module` | UNTESTED | Fixed 2026-03-07: rewires stack-flow pins only, preserves overrides |
| `set_module_enabled` | UNTESTED | |
| `set_module_input_value` | UNTESTED | Test each type: float, int, bool, vec2/3/4, color, string |
| `set_module_input_binding` | UNTESTED | |
| `set_module_input_di` | UNTESTED | Test with config JSON |
| `create_module_from_hlsl` | SKIP | Stub — always returns error |
| `create_function_from_hlsl` | SKIP | Stub — always returns error |
| `get_all_parameters` | UNTESTED | |
| `get_user_parameters` | UNTESTED | |
| `get_parameter_value` | UNTESTED | |
| `get_parameter_type` | UNTESTED | |
| `trace_parameter_binding` | UNTESTED | |
| `add_user_parameter` | UNTESTED | |
| `remove_user_parameter` | UNTESTED | |
| `set_parameter_default` | UNTESTED | |
| `set_curve_value` | UNTESTED | |
| `add_renderer` | UNTESTED | Test each: Sprite, Mesh, Ribbon, Light, Component |
| `remove_renderer` | UNTESTED | |
| `set_renderer_material` | UNTESTED | |
| `set_renderer_property` | UNTESTED | Test reflection types |
| `get_renderer_bindings` | UNTESTED | |
| `set_renderer_binding` | UNTESTED | Test primary + fallback ImportText format |
| `batch_execute` | UNTESTED | Test multiple ops in single transaction |
| `create_system_from_spec` | UNTESTED | Test full JSON spec with all sub-elements |
| `get_di_functions` | UNTESTED | |
| `get_compiled_gpu_hlsl` | UNTESTED | Requires GPU emitter with compiled script |

### MonolithEditor (namespace: "editor")

| Action | Status | Notes |
|--------|--------|-------|
| `trigger_build` | UNTESTED | Windows-only. Test with/without `wait` param |
| `live_compile` | UNTESTED | Added 2026-03-07. Trigger Live Coding hot-reload compile |
| `get_build_errors` | UNTESTED | Fixed 2026-03-07: uses ELogVerbosity enum instead of substring matching |
| `get_build_status` | UNTESTED | |
| `get_build_summary` | UNTESTED | |
| `search_build_output` | UNTESTED | |
| `get_recent_logs` | UNTESTED | Test default + max (1000) |
| `search_logs` | UNTESTED | Test pattern + category + verbosity filters |
| `tail_log` | UNTESTED | |
| `get_log_categories` | UNTESTED | |
| `get_log_stats` | UNTESTED | |
| `get_crash_context` | UNTESTED | Requires crash files to exist |

### MonolithConfig (namespace: "config")

| Action | Status | Notes |
|--------|--------|-------|
| `resolve_setting` | UNTESTED | Test with Engine/Game/Input categories |
| `explain_setting` | UNTESTED | Test convenience mode (just `setting` param, no file/section/key) |
| `diff_from_default` | UNTESTED | Fixed 2026-03-07: uses GConfig API, supports 5 INI layers (Base, Default, Project, User, Saved) |
| `search_config` | UNTESTED | Test with and without category filter |
| `get_section` | UNTESTED | Test shortname resolution |
| `get_config_files` | UNTESTED | Test with and without category filter |

### MonolithIndex (namespace: "project")

| Action | Status | Notes |
|--------|--------|-------|
| `search` | UNTESTED | Test FTS5 syntax: exact, prefix*, "phrase", OR, NOT, NEAR/N |
| `find_references` | UNTESTED | Test bidirectional (deps + referencers) |
| `find_by_type` | UNTESTED | Test pagination (limit + offset) |
| `get_stats` | UNTESTED | Should return row counts for 11 tables |
| `get_asset_details` | UNTESTED | Test deep inspection: nodes, variables, references |

### MonolithSource (namespace: "source")

| Action | Status | Notes |
|--------|--------|-------|
| `read_source` | UNTESTED | Test exact match, FTS fallback, members_only mode, max_lines |
| `find_references` | UNTESTED | Test ref_kind filter |
| `find_callers` | UNTESTED | |
| `find_callees` | UNTESTED | |
| `search_source` | UNTESTED | Test scope expansion: "cpp" -> {header,source,inline}, "shaders" -> {shader,shader_header} |
| `get_class_hierarchy` | UNTESTED | Test direction: both/ancestors/descendants. Depth limit. Max 80 truncation |
| `get_module_info` | UNTESTED | Test top 20 key classes |
| `get_symbol_context` | UNTESTED | Test context_lines param |
| `read_file` | UNTESTED | Test path resolution: absolute -> DB exact -> suffix match |
| `trigger_reindex` | UNTESTED | Requires Python 3.10+ with tree-sitter installed |

---

## Integration Tests

| Test | Status | Notes |
|------|--------|-------|
| Plugin loads without errors | UNTESTED | Check LogMonolith for startup messages |
| MCP server starts on configured port | UNTESTED | Verify `tools/list` returns all tools |
| Project auto-indexes on first launch | UNTESTED | Check notification bar + DB file creation |
| All 9 modules register their actions | UNTESTED | `monolith_discover` should list all namespaces |
| CORS headers present | UNTESTED | Check `Access-Control-Allow-Origin: *` |
| Stateless server (no session tracking) | UNTESTED | Verify server accepts requests without session headers, verify no Mcp-Session-Id in responses |
| Batch JSON-RPC requests | UNTESTED | Send array of requests |
| Invalid JSON handling | UNTESTED | Should return -32700 Parse error |
| Unknown method handling | UNTESTED | Should return -32601 Method not found |
| Undo/redo after write operations | UNTESTED | Test material build, niagara add_emitter, animation add_section |
| Module enable toggles functional | UNTESTED | Disable a module in settings, verify its actions are not registered on restart |
| Cross-platform update extraction | UNTESTED | Test update install on Windows (PowerShell), macOS/Linux (unzip) |
| Hot-swap plugin on editor exit | UNTESTED | Stage an update, exit editor, verify plugin files are replaced |
| Incremental indexing | UNTESTED | Add/remove/rename an asset, verify index updates via Asset Registry callbacks |
| Deep asset indexing (game-thread) | UNTESTED | Verify deep indexing batches run on game thread without editor hitches |
| 7 new indexers register | UNTESTED | Verify Animation, Niagara, DataTable, Level, GameplayTag, Config, Cpp indexers produce data |
| editor.live_compile action | UNTESTED | Trigger live compile via MCP, verify hot-reload occurs |
| diff_from_default 5 INI layers | UNTESTED | Test diff across Base, Default, Project, User, Saved layers |
| reorder_emitters change notifications | UNTESTED | Reorder emitters, verify PostEditChange fires and asset is marked dirty |

---

## Performance Benchmarks

| Benchmark | Status | Target | Notes |
|-----------|--------|--------|-------|
| Full project index time | UNTESTED | < 60s for 10K assets | Currently metadata-only |
| FTS5 search latency | UNTESTED | < 100ms | |
| Source DB query latency | UNTESTED | < 200ms | |
| Material graph export (complex) | UNTESTED | < 2s | |
| Niagara create_system_from_spec | UNTESTED | < 5s | |
| HTTP request/response overhead | UNTESTED | < 10ms | |
| Memory usage (idle) | UNTESTED | < 50MB | 2 SQLite DBs open |

---

## Test History

| Date | Tester | Scope | Result | Notes |
|------|--------|-------|--------|-------|
| 2026-03-06 | tumourlove + Claude | Full build | PASS | All 9 modules compile clean on UE 5.7 |
| 2026-03-07 | tumourlove + Claude | Bug fixes | PASS | HTTP body null-term, Niagara graph traversal, emitter lookup, Source DB WAL, asset loading, SQL schema creation, reindex dispatch |
| 2026-03-07 | tumourlove + Claude | 8 bug fixes | PASS | remove_bone_track (RemoveBoneTrack API), last_full_index (WriteMeta), move_module (pin rewire), get_build_errors (ELogVerbosity), SQL parameterization (13 methods), LogTemp->LogMonolith, CachedLogCapture safety, MonolithSource flatten. Build: 0 errors, 3.95s |
| 2026-03-07 | tumourlove + Claude | Session + first-call fixes | PASS | Removed session tracking from HTTP server (fully stateless). Fixed first-tool-call failures: transport type mismatch in .mcp.json ("http" → "streamableHttp") + MonolithSource stub not registering actions |
| 2026-03-07 | tumourlove + Claude | Waves 1-4 features | PASS | Module enable toggles enforced, editor.live_compile added, diff_from_default GConfig+5 layers, Niagara reorder_emitters PostEditChange+MarkPackageDirty, cross-platform update extraction, hot-swap plugin on exit, 7 new indexers (Animation/Niagara/DataTable/Level/GameplayTag/Config/Cpp), incremental indexing with Asset Registry callbacks, deep asset indexing with game-thread batching |

---

## Regression Checklist

Before any release, verify:

- [ ] All 9 modules compile clean (`UBT LeviathanEditor Win64 Development`)
- [ ] MCP server starts and responds to `tools/list`
- [ ] `monolith_discover` lists all 9 namespaces
- [ ] `monolith_status` returns correct version
- [ ] `project.search` returns results
- [ ] `source.read_source` returns code for a known class (e.g., "AActor")
- [ ] `blueprint.list_graphs` works on a test Blueprint
- [ ] `material.get_all_expressions` works on a test Material
- [ ] `editor.get_recent_logs` returns log entries
- [ ] `config.resolve_setting` returns a known setting value
- [ ] First-tool-call works on fresh session — `tools/list` returns all actions on first call, `source.query` works without retry
- [ ] MonolithSource registers all 10 actions — verify all callable: read_source, find_references, find_callers, find_callees, search_source, get_class_hierarchy, get_module_info, get_symbol_context, read_file, trigger_reindex
- [ ] Module enable toggle disables action registration when set to false
- [ ] `editor.live_compile` triggers Live Coding compile
- [ ] Incremental index updates on asset add/remove/rename
- [ ] Deep indexing produces data for Animation, Niagara, DataTable, Level assets
- [ ] No LogMonolith errors in editor log on clean startup
