import { writable, derived } from 'svelte/store';

export const boardDefs = writable([]);
export const boardPositions = writable({});
export const boardLayout = writable([]);
export const boardLoaded = writable(false);

export const boardRows = derived(
  [boardDefs, boardPositions],
  ([$boardDefs, $boardPositions]) => {
    return $boardDefs.map((mod) => ({
      address: mod.address,
      track: mod.track ?? 'Ungrouped',
      label: mod.label,
      baseType: mod.baseType,
      length: Math.max(1, Number(mod.length ?? 1) || 1),
      positions: mod.positions,
      defaultPosition: mod.defaultPosition ?? 0,
      selectedPosition: $boardPositions[String(mod.address)] ?? mod.defaultPosition ?? 0,
    }));
  }
);

export const boardTrackGroups = derived(boardRows, ($boardRows) => {
  const grouped = $boardRows.reduce((acc, row) => {
    const key = String(row.track ?? 'Ungrouped');
    if (!acc[key]) acc[key] = [];
    acc[key].push(row);
    return acc;
  }, {});

  return Object.keys(grouped)
    .sort((a, b) => a.localeCompare(b))
    .map((track) => ({ track, rows: grouped[track] }));
});

export async function loadBoardData() {
  const [defsResp, configResp, layoutResp] = await Promise.all([
    fetch('/ui/config/board_modules.json'),
    fetch('/config'),
    fetch('/ui/config/modules.json'),
  ]);

  const defs = await defsResp.json();
  const config = await configResp.json();
  const layout = await layoutResp.json();

  boardDefs.set(defs.modules || []);
  boardPositions.set(config.modulePositions || {});
  boardLayout.set(layout.modules || []);
  boardLoaded.set(true);
}

export async function saveBoardPositions(positions) {
  const payload = encodeURIComponent(JSON.stringify({ modulePositions: positions }));
  const response = await fetch('/config', {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: `payload=${payload}`,
  });
  return response.json();
}
