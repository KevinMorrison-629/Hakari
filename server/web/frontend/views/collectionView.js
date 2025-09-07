import { renderCard } from '../ui/card.js';

// --- MODULE-LEVEL STATE ---
let fullInventory = [];
let sortState = { key: 'name', direction: 'asc' };
let searchTerm = '';
let cardOptions = {};

// --- PRIVATE FUNCTIONS ---

/**
 * Renders the grid of cards based on the currently filtered and sorted inventory.
 * @param {Array<object>} inventory - The array of cards to display.
 */
function renderCardGrid(inventory) {
    const gridContainer = document.getElementById('collection-grid-container');
    if (!gridContainer) return;

    if (inventory && inventory.length > 0) {
        gridContainer.innerHTML = inventory.map(card => renderCard(card, cardOptions)).join('');
    } else {
        gridContainer.innerHTML = `<p class="list-placeholder">No cards match the current filters.</p>`;
    }
}

/**
 * Applies search and sort filters to the full inventory and re-renders the grid.
 */
function updateAndRender() {
    let processedInventory = [...fullInventory];
    const term = searchTerm.toLowerCase();

    if (term) {
        processedInventory = processedInventory.filter(card =>
            card.name.toLowerCase().includes(term) ||
            card.number.toString().includes(term)
        );
    }

    processedInventory.sort((a, b) => {
        const { key, direction } = sortState;
        const valA = a[key];
        const valB = b[key];
        let comparison = (typeof valA === 'number' && typeof valB === 'number')
            ? valA - valB
            : valA.toString().localeCompare(valB.toString());
        return direction === 'asc' ? comparison : -comparison;
    });

    renderCardGrid(processedInventory);
}

// --- PUBLIC RENDER FUNCTION ---

/**
 * Renders a complete, interactive collection view with searching and sorting.
 * @param {HTMLElement} container - The element to render the view into.
 * @param {Array<object>} inventoryData - The initial array of card objects to display.
 * @param {object} config - Configuration for the collection view.
 * @param {string} config.title - The main title for the collection view (e.g., "My Collection").
 * @param {string} [config.headerHTML=''] - Optional extra HTML to inject into the header (e.g., a back button).
 * @param {object} [config.cardOptions={}] - Options to pass down to each rendered card (e.g., { isDraggable: true }).
 */
export function renderCollectionView(container, inventoryData, config) {
    // 1. Reset state for the new view
    fullInventory = inventoryData || [];
    sortState = { key: 'name', direction: 'asc' };
    searchTerm = '';
    cardOptions = config.cardOptions || {};

    // 2. Render the static HTML layout
    container.innerHTML = `
        <div class="collection-view-container">
            <div class="collection-header">
                ${config.headerHTML || ''}
                <h1 class="collection-title">${config.title}</h1>
                <div class="collection-controls">
                    <input type="search" id="collection-search" class="form-input" placeholder="Search cards...">
                    <select id="collection-sort-key" class="form-input">
                        <option value="name">Sort by Name</option>
                        <option value="number">Sort by Number</option>
                    </select>
                    <button id="collection-sort-dir" class="btn">▲</button>
                </div>
            </div>
            <div id="collection-grid-container" class="card-grid"></div>
        </div>
    `;

    // 3. Initial render of the cards
    updateAndRender();

    // 4. Attach event listeners
    document.getElementById('collection-search').addEventListener('input', (e) => {
        searchTerm = e.target.value;
        updateAndRender();
    });

    document.getElementById('collection-sort-key').addEventListener('change', (e) => {
        sortState.key = e.target.value;
        updateAndRender();
    });

    document.getElementById('collection-sort-dir').addEventListener('click', (e) => {
        sortState.direction = sortState.direction === 'asc' ? 'desc' : 'asc';
        e.currentTarget.innerHTML = sortState.direction === 'asc' ? '▲' : '▼';
        updateAndRender();
    });
}
