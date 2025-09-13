import { logout } from '../auth.js';
import { apiFetch } from '../api/api.js';
import { renderInventoryView } from './inventoryView.js';
import { renderFriendsView } from './friendsView.js';
import { renderStoreView } from './storeView.js';

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
            renderStoreView(contentContainer);
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
