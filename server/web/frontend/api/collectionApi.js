import { apiFetch } from './api.js';

/**
 * Fetches the user's complete collection, including inventory and decks.
 * @returns {Promise<object>} The collection data.
 */
export async function loadCollectionData() {
    const res = await apiFetch('/api/collection');
    if (!res.ok) {
        const errorData = await res.json().catch(() => ({ message: 'Failed to fetch collection.' }));
        throw new Error(errorData.message);
    }
    return res.json();
}

/**
 * Saves the state of a specific deck.
 * @param {number} deckIndex The index of the deck to save.
 * @param {string[]} cards An array of card object IDs in the deck.
 * @returns {Promise<object>} The response data from the server.
 */
export async function saveActiveDeck(deckIndex, cards) {
    const res = await apiFetch('/api/decks', {
        method: 'PUT',
        body: JSON.stringify({
            deckIndex,
            cards
        })
    });
    if (!res.ok) {
        const errorData = await res.json().catch(() => ({ message: 'Failed to save deck.' }));
        throw new Error(errorData.message);
    }
    return res.json();
}
