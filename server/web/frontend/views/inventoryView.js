import { loadCollectionData, saveActiveDeck } from '../api/collectionApi.js';
import { showNotification } from '../ui/notification.js';

// Module-level state
let fullInventory = [];
let decks = [];
let filteredInventory = [];

let activeDeckIndex = -1;
let isEditing = false;
let editingDeck = []; // A temporary copy of the deck being edited

/**
 * Main function to render the entire inventory and deck building view.
 * @param {HTMLElement} container The element to render the view into.
 */
export async function renderInventoryView(container) {
    container.innerHTML = `
        <div class="inventory-layout">
            <div class="deck-builder-section">
                <h1>Deck Builder</h1>
                <div id="deck-controls" class="deck-controls"></div>
                <div id="deck-hotbar" class="deck-hotbar"></div>
            </div>
            <div class="inventory-section">
                <div class="inventory-header">
                    <h1>My Collection</h1>
                    <input type="search" id="inventory-search" class="form-input" placeholder="Search for cards..." />
                </div>
                <div id="inventory-grid-container" class="card-grid">Loading your collection...</div>
            </div>
        </div>
        <div id="card-context-menu" class="card-context-menu" style="display: none;"></div>
    `;

    await fetchAndRenderCollection();

    // Default to the first deck being selected on load.
    if (decks.length > 0) {
        activeDeckIndex = 0;
    }
    renderDeckControls();
    renderDeckHotbar();
    updateAndRenderInventory();
    attachEventListeners();
}

/**
 * Fetches inventory and deck data and populates the module-level state.
 */
async function fetchAndRenderCollection() {
    const gridContainer = document.getElementById('inventory-grid-container');
    try {
        const data = await loadCollectionData();
        if (data.success) {
            fullInventory = data.inventory || [];
            decks = data.decks || [];
        } else {
            gridContainer.innerHTML = `<div class="message-box message-error">${data.message}</div>`;
        }
    } catch (error) {
        gridContainer.innerHTML = `<div class="message-box message-error">${error.message}</div>`;
    }
}

/**
 * Applies search and deck filters to the inventory and re-renders the grid.
 */
function updateAndRenderInventory() {
    const searchTerm = document.getElementById('inventory-search')?.value.toLowerCase() || '';

    let tempFiltered = fullInventory.filter(card =>
        card.name.toLowerCase().includes(searchTerm) ||
        card.number.toString().includes(searchTerm)
    );

    if (isEditing && activeDeckIndex > -1) {
        const deckCardIds = new Set(editingDeck.filter(id => id));
        tempFiltered = tempFiltered.filter(card => !deckCardIds.has(card.id));
    }

    filteredInventory = tempFiltered;
    renderInventoryGrid();
}

/**
 * Renders the deck selection dropdown and action buttons.
 */
function renderDeckControls() {
    const controlsContainer = document.getElementById('deck-controls');
    if (!controlsContainer) return;
    const deckOptions = decks.map((_, index) => `<option value="${index}" ${index === activeDeckIndex ? 'selected' : ''}>Deck ${index + 1}</option>`).join('');
    controlsContainer.innerHTML = `
        <select id="deck-select" class="form-input">
            ${deckOptions}
        </select>
        <button id="edit-deck-btn" class="btn">${isEditing ? 'Cancel Edit' : 'Edit Deck'}</button>
        <button id="save-deck-btn" class="btn btn-primary" style="display: none;">Save Deck</button>
    `;
}

/**
 * Renders the 10 slots for the currently active deck.
 */
function renderDeckHotbar() {
    const hotbarContainer = document.getElementById('deck-hotbar');
    if (!hotbarContainer) return;

    hotbarContainer.innerHTML = '';
    const currentDeck = isEditing ? editingDeck : (decks[activeDeckIndex] || []);

    for (let i = 0; i < 10; i++) {
        const cardObjectId = currentDeck[i];
        const card = cardObjectId ? findCardInInventory(cardObjectId) : null;
        const slot = document.createElement('div');
        slot.className = 'deck-slot';
        slot.dataset.slotIndex = i;

        if (card) {
            slot.innerHTML = `<img src="${card.image}" alt="${card.name}" title="${card.name} (#${card.number})">`;
            slot.dataset.cardId = card.id;
        } else {
            slot.innerHTML = `<span>${i + 1}</span>`;
        }
        hotbarContainer.appendChild(slot);
    }

    hotbarContainer.classList.toggle('editing', isEditing);

    if (isEditing) {
        attachRightClickListenersToHotbar();
        attachDragListenersToHotbar();
    }
}

/**
 * Renders the grid of cards based on the currently filtered inventory.
 */
function renderInventoryGrid() {
    const gridContainer = document.getElementById('inventory-grid-container');
    if (!gridContainer) return;

    if (filteredInventory.length > 0) {
        gridContainer.innerHTML = filteredInventory.map(card => `
            <div class="inventory-card" draggable="${isEditing}" data-card-id="${card.id}">
                <img src="${card.image}" alt="${card.name}" class="card-image" />
                <h3>${card.name}</h3>
                <p># ${card.number}</p>
            </div>
        `).join('');
    } else if (fullInventory.length === 0) {
        gridContainer.innerHTML = `<p>Your inventory is empty. Open a pack in the store to get started!</p>`;
    } else {
        gridContainer.innerHTML = `<p>No available cards match your search.</p>`;
    }

    if (isEditing) {
        attachDragListenersToInventory();
        attachRightClickListenersToInventory();
    }
}

/**
 * Attaches all necessary event listeners for the view.
 */
function attachEventListeners() {
    document.getElementById('inventory-search')?.addEventListener('input', updateAndRenderInventory);
    const controlsContainer = document.getElementById('deck-controls');
    controlsContainer?.addEventListener('click', handleDeckControlClicks);
    controlsContainer?.addEventListener('change', handleDeckSelectionChange);
    document.addEventListener('click', () => { document.getElementById('card-context-menu').style.display = 'none'; });
    document.getElementById('card-context-menu').addEventListener('click', handleContextMenuAction);
}

function handleDeckControlClicks(e) { if (e.target.id === 'edit-deck-btn') toggleEditMode(); if (e.target.id === 'save-deck-btn') handleSaveDeck(); }
function handleDeckSelectionChange(e) { if (e.target.id === 'deck-select') { if (isEditing) toggleEditMode(); activeDeckIndex = parseInt(e.target.value, 10); renderDeckHotbar(); } }

function toggleEditMode() {
    isEditing = !isEditing;
    if (isEditing && activeDeckIndex > -1) {
        editingDeck = [...(decks[activeDeckIndex] || [])];
    } else {
        editingDeck = [];
    }
    document.getElementById('save-deck-btn').style.display = isEditing ? 'flex' : 'none';
    document.getElementById('deck-select').disabled = isEditing;
    document.getElementById('edit-deck-btn').textContent = isEditing ? 'Cancel Edit' : 'Edit Deck';
    renderDeckHotbar();
    updateAndRenderInventory();
}

async function handleSaveDeck() {
    const cardIdsInDeck = editingDeck.filter(id => id);
    try {
        const data = await saveActiveDeck(activeDeckIndex, cardIdsInDeck);
        if (data.success) {
            toggleEditMode();
            await fetchAndRenderCollection();
            renderDeckControls();
            renderDeckHotbar();
            showNotification('Deck saved successfully!');
        } else {
            showNotification(data.message, true);
        }
    } catch (error) {
        showNotification(error.message, true);
    }
}

function attachDragListenersToInventory() { document.querySelectorAll('.inventory-card[draggable="true"]').forEach(card => { card.addEventListener('dragstart', e => { e.dataTransfer.setData('text/plain', e.currentTarget.dataset.cardId); e.dataTransfer.effectAllowed = 'copy'; }); }); }
function attachDragListenersToHotbar() { document.querySelectorAll('.deck-slot').forEach(slot => { slot.addEventListener('dragover', e => e.preventDefault()); slot.addEventListener('drop', handleCardDrop); }); }

function handleCardDrop(e) {
    e.preventDefault();
    const cardId = e.dataTransfer.getData('text/plain');
    const targetSlot = e.currentTarget;
    const slotIndex = parseInt(targetSlot.dataset.slotIndex, 10);
    const currentCardCount = editingDeck.filter(id => id).length;

    if (currentCardCount >= 10 && !editingDeck[slotIndex]) {
        showNotification('Deck is full! Cannot add more than 10 cards.', true);
        return;
    }
    editingDeck[slotIndex] = cardId;
    renderDeckHotbar();
    updateAndRenderInventory();
}

function attachRightClickListenersToInventory() { document.querySelectorAll('.inventory-card[draggable="true"]').forEach(card => { card.addEventListener('contextmenu', e => { e.preventDefault(); showContextMenu(e, 'inventory', { cardId: e.currentTarget.dataset.cardId }); }); }); }
function attachRightClickListenersToHotbar() { document.querySelectorAll('.deck-slot').forEach(slot => { slot.addEventListener('contextmenu', e => { e.preventDefault(); if (slot.dataset.cardId) { showContextMenu(e, 'deck', { cardId: slot.dataset.cardId, slotIndex: slot.dataset.slotIndex }); } }); }); }

function showContextMenu(e, type, data) {
    const menu = document.getElementById('card-context-menu');
    menu.innerHTML = type === 'inventory'
        ? `<div class="menu-item" data-action="add" data-card-id="${data.cardId}">Add to Deck</div>`
        : `<div class="menu-item" data-action="remove" data-slot-index="${data.slotIndex}">Remove from Deck</div>`;
    menu.style.top = `${e.clientY}px`;
    menu.style.left = `${e.clientX}px`;
    menu.style.display = 'block';
}

function handleContextMenuAction(e) {
    const { action, cardId, slotIndex } = e.target.dataset;
    if (!action) return;

    if (action === 'add') {
        const currentCardCount = editingDeck.filter(id => id).length;
        if (currentCardCount >= 10) {
            showNotification('Deck is full! Cannot add more than 10 cards.', true);
            return;
        }
        const firstEmptySlot = editingDeck.findIndex(id => !id);
        if (firstEmptySlot !== -1) {
            editingDeck[firstEmptySlot] = cardId;
        } else {
            editingDeck.push(cardId) // Fallback if deck is not full but has no empty slots (sparse)
        }
    } else if (action === 'remove') {
        delete editingDeck[parseInt(slotIndex, 10)];
    }

    renderDeckHotbar();
    updateAndRenderInventory();
    document.getElementById('card-context-menu').style.display = 'none';
}

function findCardInInventory(cardId) { return fullInventory.find(c => c.id === cardId); }
