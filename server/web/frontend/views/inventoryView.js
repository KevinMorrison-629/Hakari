import { getFriendsData } from '../api/friendsApi.js';
import { loadCollectionData, saveActiveDeck } from '../api/collectionApi.js';
import { showNotification } from '../ui/notification.js';
import { renderCollectionView } from './collectionView.js';

// --- MODULE-LEVEL STATE ---
let fullInventory = []; // Inventory of the currently selected user
let decks = [];         // The player's own decks
let friends = [];       // The player's friends list
let activeDeckIndex = -1;
let isEditing = false;
let editingDeck = [];

/**
 * Maps card data from the backend schema to the schema expected by the CardUI component.
 */
function mapCardDataForView(card) {
    if (!card) return null;
    return {
        id: card.id,
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
 * Central UI update function for deck editing.
 * Renders available cards and the deck being edited.
 */
function refreshEditView() {
    if (!isEditing) return;

    const editingDeckCardIds = new Set(editingDeck.filter(id => id));
    const availableInventory = fullInventory.filter(card => !editingDeckCardIds.has(card.id));
    const mappedInventory = availableInventory.map(mapCardDataForView);

    // Rebuild dropdown HTML to persist it while disabled during editing
    let ownerOptions = `<option value="@me" selected>My Collection</option>`;
    ownerOptions += friends.map(friend => `<option value="${friend._id}">${friend.displayName}</option>`).join('');
    const headerDropdownHTML = `
        <div class="collection-owner-controls" style="padding-bottom: 1rem;">
            <label for="collection-owner-select" style="margin-right: 0.5rem;">Viewing:</label>
            <select id="collection-owner-select" class="form-input" disabled>${ownerOptions}</select>
        </div>`;

    renderCollectionView(document.getElementById('collection-view-wrapper'), mappedInventory, {
        title: 'My Collection',
        headerHTML: headerDropdownHTML,
        cardOptions: { isDraggable: true }
    });

    attachDragListenersToInventory();
    attachRightClickListenersToInventory();
    renderDeckHotbar();
}

/**
 * Main function to render the inventory and deck building view.
 * @param {HTMLElement} container The element to render the view into.
 * @param {object} [initialOwner] The owner to display initially.
 */
export async function renderInventoryView(container, initialOwner = { userId: '@me', userName: 'My Collection' }) {
    container.innerHTML = `
        <div class="inventory-layout">
            <div id="deck-builder-section" class="deck-builder-section">
                <h1>Deck Builder</h1>
                <div id="deck-controls" class="deck-controls"></div>
                <div id="deck-hotbar" class="deck-hotbar"></div>
            </div>
            <div id="collection-view-wrapper" class="inventory-section"></div>
        </div>
        <div id="card-context-menu" class="card-context-menu" style="display: none;"></div>
    `;
    attachMainEventListeners(container);
    await initializeInventory(initialOwner);
}

/**
 * Fetches initial data (friends, user collection) and renders the view.
 * @param {object} owner The owner to display initially.
 */
async function initializeInventory(owner) {
    const wrapper = document.getElementById('collection-view-wrapper');
    wrapper.innerHTML = `<div class="loader">Loading data...</div>`;
    try {
        const friendsData = await getFriendsData();
        if (!friendsData.success) throw new Error(friendsData.message);
        friends = friendsData.friends || [];
        await loadAndRenderCollection(owner.userId, owner.userName);
    } catch (error) {
        wrapper.innerHTML = `<div class="message-box message-error">${error.message}</div>`;
    }
}

/**
 * Fetches and renders the collection for a specific user.
 * @param {string} userId The ID of the user whose collection to load ('@me' for self).
 * @param {string} userName The display name of the user.
 */
async function loadAndRenderCollection(userId, userName) {
    const isOwner = userId === '@me';
    const wrapper = document.getElementById('collection-view-wrapper');
    const deckBuilderSection = document.getElementById('deck-builder-section');

    wrapper.innerHTML = `<div class="loader">Loading ${userName}'s collection...</div>`;
    deckBuilderSection.style.display = isOwner ? '' : 'none';

    try {
        const data = await loadCollectionData(userId);
        if (!data.success) throw new Error(data.message);

        fullInventory = data.inventory || [];
        const mappedInventory = fullInventory.map(mapCardDataForView);

        let ownerOptions = `<option value="@me" ${isOwner ? 'selected' : ''}>My Collection</option>`;
        ownerOptions += friends.map(friend =>
            `<option value="${friend._id}" ${userId === friend._id ? 'selected' : ''}>${friend.displayName}</option>`
        ).join('');

        const headerDropdownHTML = `
            <div class="collection-owner-controls" style="padding-bottom: 1rem;">
                <label for="collection-owner-select" style="margin-right: 0.5rem;">Viewing:</label>
                <select id="collection-owner-select" class="form-input">${ownerOptions}</select>
            </div>`;

        renderCollectionView(wrapper, mappedInventory, {
            title: `${userName}'s Collection`,
            headerHTML: headerDropdownHTML,
            cardOptions: { isDraggable: isOwner && isEditing }
        });

        document.getElementById('collection-owner-select').addEventListener('change', handleOwnerChange);

        if (isOwner) {
            decks = data.decks || [];
            if (activeDeckIndex === -1 && decks.length > 0) activeDeckIndex = 0;
            renderDeckControls();
            renderDeckHotbar();
        }

    } catch (error) {
        wrapper.innerHTML = `<div class="message-box message-error">${error.message}</div>`;
        showNotification(`Error loading collection: ${error.message}`, true);
    }
}


/**
 * Renders the deck selection and edit/save buttons, respecting the current edit state.
 */
function renderDeckControls() {
    const controlsContainer = document.getElementById('deck-controls');
    if (!controlsContainer) return;
    const deckOptions = decks.map((_, index) => `<option value="${index}" ${index === activeDeckIndex ? 'selected' : ''}>Deck ${index + 1}</option>`).join('');
    controlsContainer.innerHTML = `
        <select id="deck-select" class="form-input" ${isEditing ? 'disabled' : ''}>${deckOptions}</select>
        <button id="edit-deck-btn" class="btn">${isEditing ? 'Cancel' : 'Edit Deck'}</button>
        <button id="save-deck-btn" class="btn btn-primary" style="display: ${isEditing ? 'inline-flex' : 'none'};">Save Deck</button>
    `;
}

/**
 * Renders the 10-slot deck hotbar.
 */
function renderDeckHotbar() {
    const hotbarContainer = document.getElementById('deck-hotbar');
    if (!hotbarContainer) return;

    hotbarContainer.innerHTML = '';
    const currentDeck = isEditing ? editingDeck : (decks[activeDeckIndex] || []);

    for (let i = 0; i < 10; i++) {
        const cardObjectId = currentDeck[i];
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

/**
 * Toggles the deck editing mode on or off.
 */
function toggleEditMode() {
    isEditing = !isEditing;
    const ownerSelect = document.getElementById('collection-owner-select');
    if (ownerSelect) ownerSelect.disabled = isEditing;

    if (isEditing) {
        editingDeck = activeDeckIndex > -1 ? [...(decks[activeDeckIndex] || [])] : [];
        refreshEditView();
    } else {
        // On cancel, just reload our own collection view state without a full refetch
        editingDeck = [];
        loadAndRenderCollection('@me', 'My Collection');
    }

    // Manually update button states
    document.getElementById('save-deck-btn').style.display = isEditing ? 'inline-flex' : 'none';
    document.getElementById('deck-select').disabled = isEditing;
    document.getElementById('edit-deck-btn').textContent = isEditing ? 'Cancel' : 'Edit Deck';
}

/**
 * Saves the currently edited deck to the server, packing the array to remove empty slots.
 */
async function handleSaveDeck() {
    // Pack the deck array by creating a new, dense array containing only valid card IDs.
    // This correctly handles sparse arrays that result from removing cards from specific slots.
    const packedDeck = [];
    for (const cardId of editingDeck) {
        if (cardId) {
            packedDeck.push(cardId);
        }
    }

    try {
        const data = await saveActiveDeck(activeDeckIndex, packedDeck);
        if (data.success) {
            showNotification('Deck saved!');
            isEditing = false;
            await initializeInventory({ userId: '@me', userName: 'My Collection' });
        } else {
            showNotification(data.message, true);
        }
    } catch (error) {
        showNotification(error.message, true);
    }
}

// --- EVENT HANDLERS & LISTENERS ---

/**
 * Attaches the main, non-dynamic event listeners for the view.
 */
function attachMainEventListeners(container) {
    container.addEventListener('click', (e) => {
        if (e.target.id === 'edit-deck-btn') toggleEditMode();
        if (e.target.id === 'save-deck-btn') handleSaveDeck();
    });
    container.addEventListener('change', (e) => {
        if (e.target.id === 'deck-select' && !isEditing) {
            activeDeckIndex = parseInt(e.target.value, 10);
            renderDeckHotbar();
        }
    });

    document.addEventListener('click', () => {
        const menu = document.getElementById('card-context-menu');
        if (menu) menu.style.display = 'none';
    });
    const contextMenu = document.getElementById('card-context-menu');
    if (contextMenu) contextMenu.addEventListener('click', handleContextMenuAction);
}

/**
 * Handles changing the selected user in the collection dropdown.
 */
async function handleOwnerChange(e) {
    const selectedId = e.target.value;
    const selectedName = e.target.options[e.target.selectedIndex].text;
    await loadAndRenderCollection(selectedId, selectedName);
}

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
    refreshEditView();
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
        delete editingDeck[parseInt(slotIndex, 10)];
    }

    refreshEditView();
    document.getElementById('card-context-menu').style.display = 'none';
}

