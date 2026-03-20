<script>
  import { onMount } from "svelte";
  import { boardRows, boardTrackGroups, boardLayout, boardLoaded, loadBoardData, saveBoardPositions } from "./store/state";
  import { wiTitle, wiStrings, wiVersion } from "./store/webinterface";

  $: {
    document.title = $wiTitle;
  }

  let activePage = "config";
  let zeroAddress = "";
  let stepAddress = "";
  let typeAddress = "";
  let typeResult = null;
  let saveStatus = null;
  let selectedPositions = {};

  $: if ($boardLoaded && Object.keys(selectedPositions).length === 0) {
    selectedPositions = Object.fromEntries(
      $boardRows.map((r) => [String(r.address), r.selectedPosition])
    );
  }

  $: boardRowsByAddress = new Map($boardRows.map((r) => [String(r.address), r]));
  $: boardVisualization = ($boardLayout || []).map((row) => {
    const cells = [];
    let pendingFillers = 0;

    for (const raw of row) {
      const token = String(raw);
      const isFiller = token.toUpperCase() === "X";

      // Consume explicit X placeholders for previous multi-cell modules.
      if (pendingFillers > 0 && isFiller) {
        pendingFillers--;
        continue;
      }

      // If we expected fillers but got a real token, stop consuming and render it.
      if (pendingFillers > 0 && !isFiller) {
        pendingFillers = 0;
      }

      if (isFiller) {
        cells.push({ addr: "X", text: "", missing: false, filler: true, colspan: 1 });
        continue;
      }

      const moduleRow = boardRowsByAddress.get(token);
      if (!moduleRow) {
        cells.push({ addr: token, text: "-", missing: true, filler: false, colspan: 1 });
        continue;
      }

      const pos = selectedPositions[token] ?? moduleRow.selectedPosition;
      const text = moduleRow.positions?.[pos]?.label ?? "";
      const span = Math.max(1, Number(moduleRow.length ?? 1) || 1);

      cells.push({ addr: token, text, missing: false, filler: false, colspan: span });
      pendingFillers = span - 1;
    }

    return cells;
  });

  const loadWebinterfaceConfig = async () => {
    return fetch("/ui/config/webinterface.json")
      .then((response) => response.json())
      .then((data) => {
        $wiTitle = data.name;
        $wiStrings = data.strings;
        $wiVersion = data.version;
      });
  };

  const saveBoard = async () => {
    saveStatus = null;
    try {
      const result = await saveBoardPositions(selectedPositions);
      saveStatus = result.success ? "ok" : "error";
    } catch (e) {
      saveStatus = "error";
    }
  };

  const zeroAll = async () => {
    try {
      const response = await fetch("/zero");
      if (!response.ok) {
        alert("Failed to zero modules");
        return;
      }
      await response.text();
      alert("All modules zeroed successfully!");
    } catch (error) {
      console.error("Error zeroing modules:", error);
      alert("Error zeroing modules");
    }
  };

  const zeroModule = async () => {
    const addr = Number.parseInt(zeroAddress, 10);
    if (Number.isNaN(addr) || addr < 0 || addr > 255) {
      alert("Please enter a valid module address (0-255)");
      return;
    }

    try {
      const response = await fetch(`/zero?addr=${addr}`);
      if (!response.ok) {
        alert("Failed to zero module");
        return;
      }
      await response.text();
      alert(`Zeroed module ${addr}`);
    } catch (error) {
      console.error("Error zeroing module:", error);
      alert("Error zeroing module");
    }
  };

  const stepModule = async () => {
    const addr = Number.parseInt(stepAddress, 10);
    if (Number.isNaN(addr) || addr < 0 || addr > 255) {
      alert("Please enter a valid module address (0-255)");
      return;
    }

    try {
      const response = await fetch(`/step?addr=${addr}`);
      if (!response.ok) {
        alert("Failed to step module");
        return;
      }
      await response.text();
      alert(`Stepped module ${addr}`);
    } catch (error) {
      console.error("Error stepping module:", error);
      alert("Error stepping module");
    }
  };

  const getModuleType = async () => {
    const addr = Number.parseInt(typeAddress, 10);
    if (Number.isNaN(addr) || addr < 0 || addr > 255) {
      alert("Please enter a valid module address (0-255)");
      return;
    }

    typeResult = null;

    try {
      const response = await fetch(`/type?addr=${addr}`);
      if (!response.ok) {
        const errText = await response.text();
        console.error("Failed to read module type:", errText);
        alert("Failed to read module type");
        return;
      }

      const data = await response.json();
      if (!data.success) {
        alert("Failed to read module type");
        return;
      }

      typeResult = data;
    } catch (error) {
      console.error("Error reading module type:", error);
      alert("Error reading module type");
    }
  };

  onMount(async () => {
    await loadWebinterfaceConfig();
    await loadBoardData();
  });
</script>

<main>
  <header>
    <h1 id="title">{$wiTitle}</h1>
  </header>
  <div class="content">
    <div class="page-tabs">
      <button
        class:active={activePage === "config"}
        on:click={() => (activePage = "config")}
      >
        Configuration
      </button>
      <button
        class:active={activePage === "debug"}
        on:click={() => (activePage = "debug")}
      >
        Debug
      </button>
    </div>

    {#if activePage === "config"}
      {#if !$boardLoaded}
        <div class="board-loading">Loading board configuration...</div>
      {:else}
        <h2 class="section-title">Board Editor</h2>
        <h3 class="track-heading">Board Visualization</h3>
        <div class="board-visual-wrap">
          <div class="board-visual">
            {#each boardVisualization as row}
              <div class="board-visual-row">
                {#each row as cell}
                  <div
                    class="board-visual-cell"
                    class:missing={cell.missing}
                    class:filler={cell.filler}
                    style={`width: ${86 * cell.colspan + 6 * (cell.colspan - 1)}px;`}
                  >
                    <div class="visual-label">{cell.text || " "}</div>
                    <div class="visual-addr">{cell.addr}</div>
                  </div>
                {/each}
              </div>
            {/each}
          </div>
        </div>

        {#each $boardTrackGroups as group}
          <h3 class="track-heading">Track {group.track}</h3>
          <div class="track-section">
            <table class="board-table">
              <thead>
                <tr>
                  <th>Addr</th>
                  <th>Label</th>
                  <th>Len</th>
                  <th>Type</th>
                  <th>Selection</th>
                  <th>Preview</th>
                </tr>
              </thead>
              <tbody>
                {#each group.rows as row}
                  <tr>
                    <td>{row.address}</td>
                    <td>{row.label}</td>
                    <td>{row.length}</td>
                    <td>{row.baseType}</td>
                    <td>
                      <select
                        value={selectedPositions[String(row.address)] ?? row.selectedPosition}
                        on:change={(e) => {
                          selectedPositions = {
                            ...selectedPositions,
                            [String(row.address)]: +e.target.value,
                          };
                        }}
                      >
                        {#each row.positions as pos, i}
                          <option value={i}>{i}{pos.label ? ': ' + pos.label : ''}</option>
                        {/each}
                      </select>
                    </td>
                    <td class="preview-cell">
                      {row.positions[selectedPositions[String(row.address)] ?? row.selectedPosition]?.label ?? ''}
                    </td>
                  </tr>
                {/each}
              </tbody>
            </table>
          </div>
        {/each}
        <div class="save-line">
          <button on:click={saveBoard}>Save</button>
          {#if saveStatus === 'ok'}
            <span class="save-status ok">Saved!</span>
          {:else if saveStatus === 'error'}
            <span class="save-status error">Save failed</span>
          {/if}
        </div>
      {/if}
    {:else}
        <h2 class="section-title">Debug Functions</h2>
        <div class="block debug-block">
          <p class="debug-intro">Use these actions for diagnostics and maintenance.</p>

          <div class="debug-option">
            <h3>Zero All Modules</h3>
            <p>
              Sends the ZERO command to all configured modules and moves each one
              to its hardware zero position.
            </p>
            <button on:click={zeroAll} class="zero-button">Run Zero All</button>
          </div>

          <div class="debug-option">
            <h3>Zero One Module</h3>
            <p>
              Sends the ZERO command to a single module address. Use this to test
              or calibrate one module without moving the whole display.
            </p>
            <div class="debug-controls">
              <input
                type="number"
                min="0"
                max="255"
                bind:value={zeroAddress}
                placeholder="Module address"
              />
              <button on:click={zeroModule} class="zero-button">Run Zero</button>
            </div>
          </div>

          <div class="debug-option">
            <h3>Step One Module</h3>
            <p>
              Sends the STEP command to one module address and advances the flap
              by one blade.
            </p>
            <div class="debug-controls">
              <input
                type="number"
                min="0"
                max="255"
                bind:value={stepAddress}
                placeholder="Module address"
              />
              <button on:click={stepModule}>Run Step</button>
            </div>
          </div>

          <div class="debug-option">
            <h3>Read Module Type</h3>
            <p>
              Sends the TYPE read command to one module and shows the returned
              module type in this page.
            </p>
            <div class="debug-controls">
              <input
                type="number"
                min="0"
                max="255"
                bind:value={typeAddress}
                placeholder="Module address"
              />
              <button on:click={getModuleType}>Read Type</button>
            </div>
            {#if typeResult}
              <p class="debug-result">
                Module {typeResult.addr}: {typeResult.typeName} ({typeResult.typeHex})
              </p>
            {/if}
          </div>
        </div>
      {/if}
  </div>
  <footer>{$wiStrings["copyright"]} &bull; {$wiVersion}</footer>
</main>

<style>
  .page-tabs {
    display: flex;
    background: #d6d6d6;
    border-bottom: 1px solid #bdbdbd;
  }

  .page-tabs button {
    flex: 1;
    border: 0;
    background: transparent;
    color: #2d327d;
    padding: 10px 12px;
    cursor: pointer;
    font-weight: 600;
  }

  .page-tabs button.active {
    background: #ffffff;
    color: #1f245f;
  }

  .save-line {
    height: 48px;
    background: #e5e5e5;
    text-align: left;
  }

  .save-line button,
  .debug-block button {
    margin: 7px 10px;
    padding: 10px 20px;
    border: 0;
    border-radius: 5px;
    box-shadow: none;
    background: #2d327d;
    color: #ffffff;
    cursor: pointer;
  }

  .board-loading {
    padding: 20px;
    color: #4a4a4a;
  }

  .board-table {
    width: 100%;
    border-collapse: collapse;
    font-size: 0.9rem;
  }

  .board-table th {
    background: #2d327d;
    color: #fff;
    padding: 8px 10px;
    text-align: left;
  }

  .board-table td {
    padding: 7px 10px;
    border-bottom: 1px solid #ddd;
  }

  .board-table tr:nth-child(even) td {
    background: #f5f5f5;
  }

  .board-table select {
    padding: 5px 8px;
    border: 1px solid #bcbcbc;
    border-radius: 4px;
    font-size: 0.9rem;
    width: 100%;
    min-width: 160px;
  }

  .preview-cell {
    font-weight: 600;
    color: #2d327d;
  }

  .track-heading {
    margin: 18px 10px 8px 10px;
    color: #2d327d;
    font-size: 1.05rem;
    border-bottom: 2px solid #d7dcf3;
    padding-bottom: 4px;
  }

  .track-section {
    margin-bottom: 14px;
  }

  .board-visual-wrap {
    margin: 0 10px 12px 10px;
    overflow-x: auto;
  }

  .board-visual {
    display: grid;
    gap: 6px;
  }

  .board-visual-row {
    display: flex;
    gap: 6px;
    width: max-content;
  }

  .board-visual-cell {
    box-sizing: border-box;
    width: 86px;
    flex: 0 0 auto;
    height: 56px;
    background: #f5f6fb;
    border: 1px solid #ccd3eb;
    border-radius: 6px;
    text-align: center;
    padding: 4px;
  }

  .board-visual-cell.missing {
    background: #f8f8f8;
    border-color: #e0e0e0;
    color: #999;
  }

  .board-visual-cell.filler {
    background: #fafafa;
    border-style: dashed;
  }

  .visual-label {
    font-weight: 700;
    color: #2d327d;
    line-height: 1.1;
    min-height: 16px;
  }

  .visual-addr {
    margin-top: 4px;
    font-size: 0.75rem;
    color: #666;
  }

  .save-status {
    margin-left: 10px;
    font-weight: 600;
  }

  .save-status.ok {
    color: #2e7d32;
  }

  .save-status.error {
    color: #c62828;
  }

  .debug-block {
    min-height: 120px;
  }

  .debug-intro {
    margin: 0 0 8px 10px;
    color: #4a4a4a;
  }

  .debug-option {
    margin: 12px 10px;
    padding: 12px;
    background: #f2f2f2;
    border: 1px solid #d8d8d8;
    border-radius: 6px;
  }

  .debug-option h3 {
    margin: 0 0 6px 0;
    color: #2d327d;
    font-size: 1rem;
  }

  .debug-option p {
    margin: 0;
    color: #4a4a4a;
    line-height: 1.4;
  }

  .debug-controls {
    margin-top: 10px;
    display: flex;
    gap: 8px;
    align-items: center;
  }

  .debug-controls input {
    width: 160px;
    padding: 9px 10px;
    border: 1px solid #bcbcbc;
    border-radius: 5px;
    font-size: 0.95rem;
  }

  .debug-result {
    margin-top: 10px;
    padding: 8px 10px;
    background: #e7f1ff;
    border: 1px solid #b8d3f7;
    border-radius: 5px;
    color: #1f245f;
    font-weight: 600;
  }

  .debug-block button.zero-button {
    background: #d97627;
  }

  .save-line button:hover,
  .debug-block button:hover {
    opacity: 0.8;
  }
</style>
