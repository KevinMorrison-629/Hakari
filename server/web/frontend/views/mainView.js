import { logout } from '../auth.js';
import { apiFetch } from '../api.js';

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
                    <div class="brand">CardForge</div>
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

    // Attach event listeners and render initial content
    attachEventListeners();
    renderContent();
}

/**
 * Renders the content for the currently active tab.
 */
function renderContent() {
    const contentContainer = document.getElementById('main-content');
    if (!contentContainer) return;

    switch (activeTab) {
        case 'Store':
            renderStoreContent(contentContainer);
            break;
        case 'Inventory':
            renderInventoryContent(contentContainer);
            break;
        default:
            renderPlaceholderContent(contentContainer, activeTab);
            break;
    }
}

/**
 * Attaches event listeners for navigation and logout.
 */
function attachEventListeners() {
    document.getElementById('logout-btn').addEventListener('click', logout);

    document.querySelectorAll('.nav-link').forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            activeTab = e.currentTarget.dataset.tab;

            // Update active class
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
                        `).join('').replace(/border-bottom: 1px solid #374151;(?![\s\S]*border-bottom: 1px solid #374151;)/, '')}
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

async function renderInventoryContent(container) {
    container.innerHTML = `
        <h1>My Inventory</h1>
        <p class="page-description">All of the cards you've collected.</p>
        <div id="inventory-grid-container">Loading your collection...</div>
    `;

    const gridContainer = document.getElementById('inventory-grid-container');

    try {
        const res = await apiFetch('/api/inventory'); // GET is default
        const data = await res.json();

        if (res.ok && data.success) {
            if (data.inventory.length > 0) {
                gridContainer.className = 'card-grid';
                gridContainer.innerHTML = data.inventory.map(card => `
                    <div class="inventory-card">
                        <img src="${card.image}" alt="${card.name}" class="card-image" />
                        <h3>${card.name}</h3>
                        <p># ${card.number}</p>
                    </div>
                `).join('');
            } else {
                gridContainer.innerHTML = `<p>Your inventory is empty. Open a pack in the store to get started!</p>`;
            }
        } else {
            gridContainer.innerHTML = `<div class="message-box message-error">${data.message || 'Failed to fetch inventory.'}</div>`;
        }
    } catch (error) {
        gridContainer.innerHTML = `<div class="message-box message-error">Could not connect to the server.</div>`;
    }
}
