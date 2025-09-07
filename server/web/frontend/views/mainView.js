import { logout } from '../auth.js';
import { apiFetch } from '../api/api.js';
import { renderInventoryView } from './inventoryView.js';
import { renderFriendsView } from './friendsView.js';

let activeTab = 'Store';

/**
 * Renders the main application view after login.
 * @param {HTMLElement} container - The element to render the view into.
 */
export function renderMainView(container) {
    const menuItems = [
        { id: 'Inventory', icon: '📦', name: 'Inventory' },
        { id: 'Store', icon: '🛒', name: 'Store' },
        { id: 'Friends', icon: '👥', name: 'Friends' },
        { id: 'Chat', icon: '💬', name: 'Chat' },
        { id: 'Leaderboard', icon: '🏆', name: 'Leaderboard' },
        { id: 'Battle', icon: '⚔️', name: 'Battle' },
        { id: 'Trading House', icon: '🔄', name: 'Trading House' },
        { id: 'Connect Discord', icon: '🔗', name: 'Connect Discord' },
    ];

    container.innerHTML = `
        <div class="main-layout">
            <nav class="sidebar">
                <div>
                    <div class="brand">HakariBot</div>
                    <ul id="nav-menu">
                        ${menuItems.map(item => `
                            <li>
                                <a href="#" data-tab="${item.id}" class="nav-link ${activeTab === item.id ? 'active' : ''}">
                                    <span class="menu-icon">${item.icon}</span>
                                    ${item.name}
                                </a>
                            </li>
                        `).join('')}
                    </ul>
                </div>
                <button id="logout-btn" class="logout-btn">
                    <span class="menu-icon">🚪</span>
                    Log Out
                </button>
            </nav>
            <main id="main-content" class="main-content"></main>
        </div>
    `;

    attachEventListeners();
    renderContent();
}

function renderContent() {
    const contentContainer = document.getElementById('main-content');
    if (!contentContainer) return;

    switch (activeTab) {
        case 'Store':
            renderStoreContent(contentContainer);
            break;
        case 'Inventory':
            renderInventoryView(contentContainer);
            break;
        case 'Friends':
            renderFriendsView(contentContainer);
            break;
        default:
            renderPlaceholderContent(contentContainer, activeTab);
            break;
    }
}

function attachEventListeners() {
    document.getElementById('logout-btn').addEventListener('click', logout);

    document.querySelectorAll('.nav-link').forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            const newTab = e.currentTarget.dataset.tab;
            if (activeTab === newTab) return; // Do nothing if clicking the same tab

            activeTab = newTab;
            document.querySelector('.nav-link.active').classList.remove('active');
            e.currentTarget.classList.add('active');
            renderContent();
        });
    });
}

// --- Content Rendering Functions ---

function renderPlaceholderContent(container, title) {
    container.innerHTML = `
        <h1>${title}</h1>
        <p class="page-description">This section is under construction. Check back soon!</p>
    `;
}

async function renderStoreContent(container) {
    container.innerHTML = `
        <h1>Store</h1>
        <p class="page-description">Purchase and open card packs to build your collection.</p>
        <div class="card" style="max-width: 24rem;">
            <h2>Standard Pack</h2>
            <p style="color: #9ca3af; margin-top: 0.25rem;">Contains 5 random cards.</p>
            <button id="open-pack-btn" class="btn btn-primary" style="margin-top: 1rem;">Open Pack</button>
        </div>
        <div id="store-message-container" style="margin-top: 1.5rem; max-width: 24rem;"></div>
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
                    <div class="card">
                        ${data.cards.map(card => `
                            <div class="inventory-card">
                                <img src="${card.image}" alt="${card.name}" class="card-image" />
                                <h3>${card.name}</h3>
                                <p># ${card.number}</p>
                            </div>
                        `).join('')}
                    </div>`;
            } else {
                messageContainer.innerHTML = `<div class="message-box message-error">${data.message || 'Failed to open pack.'}</div>`;
            }
        } catch (error) {
            messageContainer.innerHTML = `<div class="message-box message-error">An error occurred while communicating with the server.</div>`;
        } finally {
            btn.disabled = false;
            btn.innerHTML = 'Open Pack';
        }
    });
}
