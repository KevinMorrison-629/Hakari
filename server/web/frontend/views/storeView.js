import { apiFetch } from '../api/api.js';
import CardUI from '../ui/card.js';

/**
 * Renders the store content, allowing users to open card packs.
 * @param {HTMLElement} container - The element to render the view into.
 */
export async function renderStoreView(container) {
    container.innerHTML = `
        <h1>Store</h1>
        <p class="page-description">Purchase and open card packs to build your collection.</p>
        <div class="card" style="max-width: 24rem;">
            <h2>Standard Pack</h2>
            <p style="color: #9ca3af; margin-top: 0.25rem;">Contains 5 random cards.</p>
            <button id="open-pack-btn" class="btn btn-primary" style="margin-top: 1rem;">Open Pack</button>
        </div>
        <div id="store-message-container" style="margin-top: 1.5rem;"></div>
    `;

    document.getElementById('open-pack-btn').addEventListener('click', async (e) => {
        const btn = e.currentTarget;
        const messageContainer = document.getElementById('store-message-container');

        btn.disabled = true;
        btn.innerHTML = `<span class="animate-spin">🎲</span> Opening...`;
        messageContainer.innerHTML = '';

        try {
            const res = await apiFetch('/api/open_pack', { method: 'POST' });
            const data = await res.json();
            if (res.ok && data.success) {
                messageContainer.innerHTML = `
                    <h3 style="font-size: 1.125rem; font-weight: 600; margin-bottom: 0.5rem;">${data.message}</h3>
                    <div id="new-cards-container" class="grid grid-cols-2 sm:grid-cols-3 md:grid-cols-4 lg:grid-cols-5 gap-4"></div>
                `;
                const cardsContainer = document.getElementById('new-cards-container');
                data.cards.forEach(card => {
                    // NOTE: The /api/open_pack endpoint returns a simplified card object.
                    // We are creating a CardUI with default values for missing properties.
                    const cardData = {
                        id: card.number, // Using 'number' as a stand-in for a unique ID as it's not provided by this API endpoint
                        name: card.name,
                        acqNumber: card.number,
                        imageUrl: card.image,
                        ap: '?', // Not provided by API
                        hp: '?', // Not provided by API
                        ability: '', // Not provided by API
                        tier: 'common' // Not provided by API, defaulting
                    };
                    new CardUI(cardsContainer, cardData);
                });
            } else {
                messageContainer.innerHTML = `<div class="message-box message-error">${data.message || 'Failed to open pack.'}</div>`;
            }
        } catch (error) {
            console.error('Error opening pack:', error);
            messageContainer.innerHTML = `<div class="message-box message-error">An error occurred while communicating with the server.</div>`;
        } finally {
            btn.disabled = false;
            btn.innerHTML = 'Open Pack';
        }
    });
}
