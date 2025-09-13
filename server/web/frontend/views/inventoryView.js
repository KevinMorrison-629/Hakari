import { loadCollectionData, saveActiveDeck } from '../api/collectionApi.js';
import { showNotification } from '../ui/notification.js';
import { renderCollectionView } from './collectionView.js';

// Module-level state for deck building
let fullInventory = []; // Keep a copy with original backend data structure
let decks = [];
let activeDeckIndex = -1;
let isEditing = false;
let editingDeck = [];


/**
 * Maps card data from the backend schema to the schema expected by the CardUI component.
 * @param {object} card The card data from the backend.
 * @returns {object} The mapped card data for the UI.
 */
function mapCardDataForView(card) {
    if (!card) return null;
    return {
        // Pass through fields needed by other parts like drag-and-drop
        id: card.id,

        // Map fields for CardUI component
        name: card.name,
        acqNumber: card.number,
        imageUrl: card.image,
        ap: card.attackPoints,
        hp: card.healthPoints,
        tier: card.tier,
        ability: card.ability,
    };
}


/**
 * Central UI update function. Renders the available cards in the collection
 * and the current state of the deck being edited, ensuring they are always in sync.
 */
function refreshEditView() {
    if (!isEditing) return;

    // Create a Set of card IDs currently in the deck for efficient lookup.
    const editingDeckCardIds = new Set(editingDeck.filter(id => id));

    // Filter the main inventory to get only cards that are NOT in the deck.
    const availableInventory = fullInventory.filter(card => !editingDeckCardIds.has(card.id));

    // Map the data for the view before rendering
    const mappedInventory = availableInventory.map(mapCardDataForView);

    // Re-render the collection view with the filtered list of available cards.
    renderCollectionView(document.getElementById('collection-view-wrapper'), mappedInventory, {
        title: 'My Collection',
        cardOptions: { isDraggable: true }
    });

    // Re-attach event listeners to the newly rendered collection cards.
    attachDragListenersToInventory();
    attachRightClickListenersToInventory();

    // Re-render the deck hotbar to show the latest changes.
    renderDeckHotbar();
}

/**
 * Main function to render the inventory and deck building view.
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
            <div id="collection-view-wrapper" class="inventory-section">
                <!-- The Collection View will be rendered here -->
            </div>
        </div>
        <div id="card-context-menu" class="card-context-menu" style="display: none;"></div>
    `;

    // Attach event listeners ONCE using event delegation on a stable parent.
    const layout = container.querySelector('.inventory-layout');
    if (layout) {
        layout.addEventListener('click', (e) => {
            if (e.target.id === 'edit-deck-btn') toggleEditMode();
            if (e.target.id === 'save-deck-btn') handleSaveDeck();
        });
        layout.addEventListener('change', (e) => {
            if (e.target.id === 'deck-select' && !isEditing) {
                activeDeckIndex = parseInt(e.target.value, 10);
                renderDeckHotbar();
            }
        });
    }

    // Attach context menu listeners once.
    document.addEventListener('click', () => { document.getElementById('card-context-menu').style.display = 'none'; });
    document.getElementById('card-context-menu').addEventListener('click', handleContextMenuAction);


    await fetchAndDisplayData();
}

/**
 * Fetches data and renders all parts of the view.
 */
async function fetchAndDisplayData() {
    const wrapper = document.getElementById('collection-view-wrapper');
    wrapper.innerHTML = `<div class="loader">Loading your collection...</div>`;

    try {
        const data = await loadCollectionData('@me');
        if (data.success) {
            fullInventory = data.inventory || [];
            decks = data.decks || [];

            // Map the inventory data to the format the CardUI component expects.
            const mappedInventory = fullInventory.map(mapCardDataForView);

            // Initial render of the collection view with all cards.
            renderCollectionView(wrapper, mappedInventory, {
                title: 'My Collection',
                cardOptions: { isDraggable: false } // Dragging is disabled by default.
            });

            if (activeDeckIndex === -1 && decks.length > 0) {
                activeDeckIndex = 0;
            }
            renderDeckControls();
            renderDeckHotbar();

        } else {
            wrapper.innerHTML = `<div class="message-box message-error">${data.message}</div>`;
        }
    } catch (error) {
        wrapper.innerHTML = `<div class="message-box message-error">${error.message}</div>`;
    }
}

function renderDeckControls() {
    const controlsContainer = document.getElementById('deck-controls');
    if (!controlsContainer) return;
    const deckOptions = decks.map((_, index) => `<option value="${index}" ${index === activeDeckIndex ? 'selected' : ''}>Deck ${index + 1}</option>`).join('');
    controlsContainer.innerHTML = `
        <select id="deck-select" class="form-input">${deckOptions}</select>
        <button id="edit-deck-btn" class="btn">${isEditing ? 'Cancel' : 'Edit Deck'}</button>
        <button id="save-deck-btn" class="btn btn-primary" style="display: none;">Save Deck</button>
    `;
}

function renderDeckHotbar() {
    const hotbarContainer = document.getElementById('deck-hotbar');
    if (!hotbarContainer) return;

    hotbarContainer.innerHTML = '';
    const currentDeck = isEditing ? editingDeck : (decks[activeDeckIndex] || []);

    for (let i = 0; i < 10; i++) {
        const cardObjectId = currentDeck[i];
        // Find card from the original `fullInventory` to get original data structure
        const card = cardObjectId ? fullInventory.find(c => c.id === cardObjectId) : null;
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
        attachDragListenersToHotbar();
        attachRightClickListenersToHotbar();
    }
}

function toggleEditMode() {
    isEditing = !isEditing;

    if (isEditing) {
        // Entering edit mode: copy the current deck and refresh the view.
        editingDeck = activeDeckIndex > -1 ? [...(decks[activeDeckIndex] || [])] : [];
        refreshEditView();
    } else {
        // Exiting edit mode: clear the temporary deck and restore the full collection view.
        editingDeck = [];
        const mappedInventory = fullInventory.map(mapCardDataForView);
        renderCollectionView(document.getElementById('collection-view-wrapper'), mappedInventory, {
            title: 'My Collection',
            cardOptions: { isDraggable: false }
        });
        renderDeckHotbar();
    }

    // Update UI controls state.
    document.getElementById('save-deck-btn').style.display = isEditing ? 'flex' : 'none';
    document.getElementById('deck-select').disabled = isEditing;
    document.getElementById('edit-deck-btn').textContent = isEditing ? 'Cancel' : 'Edit Deck';
}

async function handleSaveDeck() {
    try {
        // Filter out any empty slots before saving.
        const data = await saveActiveDeck(activeDeckIndex, editingDeck.filter(id => id));
        if (data.success) {
            showNotification('Deck saved!');
            isEditing = false; // Manually exit edit mode before refetching data.
            await fetchAndDisplayData();
        } else {
            showNotification(data.message, true);
        }
    } catch (error) {
        showNotification(error.message, true);
    }
}

// --- DRAG & DROP AND CONTEXT MENU LOGIC ---

function attachDragListenersToInventory() {
    document.querySelectorAll('.card-component[draggable="true"]').forEach(card => {
        card.addEventListener('dragstart', e => {
            e.dataTransfer.setData('text/plain', e.currentTarget.dataset.cardId);
            e.dataTransfer.effectAllowed = 'copy';
        });
    });
}

function attachDragListenersToHotbar() {
    document.querySelectorAll('.deck-slot').forEach(slot => {
        slot.addEventListener('dragover', e => e.preventDefault());
        slot.addEventListener('drop', handleCardDrop);
    });
}

function handleCardDrop(e) {
    e.preventDefault();
    const cardId = e.dataTransfer.getData('text/plain');
    const slotIndex = parseInt(e.currentTarget.dataset.slotIndex, 10);

    if (editingDeck.includes(cardId)) {
        showNotification('Card is already in the deck!', true);
        return;
    }
    if (editingDeck.filter(id => id).length >= 10 && !editingDeck[slotIndex]) {
        showNotification('Deck is full!', true);
        return;
    }

    editingDeck[slotIndex] = cardId;
    refreshEditView(); // Update the entire view.
}

function attachRightClickListenersToInventory() {
    document.querySelectorAll('.card-component[draggable="true"]').forEach(card => {
        card.addEventListener('contextmenu', e => {
            e.preventDefault();
            showContextMenu(e, 'inventory', { cardId: e.currentTarget.dataset.cardId });
        });
    });
}

function attachRightClickListenersToHotbar() {
    document.querySelectorAll('.deck-slot').forEach(slot => {
        if (slot.dataset.cardId) {
            slot.addEventListener('contextmenu', e => {
                e.preventDefault();
                showContextMenu(e, 'deck', { slotIndex: slot.dataset.slotIndex });
            });
        }
    });
}

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

    if (action === 'add') {
        if (editingDeck.includes(cardId)) {
            showNotification('Card is already in the deck!', true);
            return;
        }
        if (editingDeck.filter(id => id).length >= 10) {
            showNotification('Deck is full!', true);
            return;
        }
        const firstEmptySlot = editingDeck.findIndex(id => !id);
        editingDeck[firstEmptySlot !== -1 ? firstEmptySlot : editingDeck.length] = cardId;
    } else if (action === 'remove') {
        // Using 'delete' creates a sparse array, which is intended here to keep slot positions stable.
        delete editingDeck[parseInt(slotIndex, 10)];
    }

    refreshEditView(); // Update the entire view.
    document.getElementById('card-context-menu').style.display = 'none';
}
