import { apiFetch } from './api.js';

/**
 * Fetches a user's collection. For the current user, use '@me' as the userId to get deck info.
 * @param {string} userId The ID of the user whose collection to fetch, or '@me' for the current user.
 * @returns {Promise<object>} The collection data, which may include a 'decks' array for the owner.
 */
export async function loadCollectionData(userId) {
    const res = await apiFetch(`/api/collection/${userId}`);
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
