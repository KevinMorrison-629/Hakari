import { getFriendsData, searchUsers, sendFriendRequest, respondToRequest, removeFriend } from '../api/friendsApi.js';
import { showNotification } from '../ui/notification.js';
import { renderInventoryView } from './inventoryView.js';

let allFriends = []; // Cache the full friends list for filtering

// --- LIST ITEM RENDERERS ---

function renderFriendListItem(user) {
    return `
        <li class="user-list-item" data-display-name="${user.displayName.toLowerCase()}">
            <span class="user-display-name">${user.displayName}</span>
            <div class="user-actions">
                <button class="btn btn-sm btn-secondary" title="View Inventory" data-id="${user._id}" data-action="inventory">📦</button>
                <button class="btn btn-sm btn-secondary" title="Send Message" data-id="${user._id}" data-action="message">💬</button>
                <button class="btn btn-sm btn-secondary" title="Trade" data-id="${user._id}" data-action="trade">🔄</button>
                <button class="btn btn-sm btn-secondary" title="Battle" data-id="${user._id}" data-action="battle">⚔️</button>
                <button class="btn btn-sm btn-danger remove-friend-btn" title="Remove Friend" data-id="${user._id}">✖</button>
            </div>
        </li>
    `;
}

function renderSearchResultItem(user) {
    let actionBtn = `<button class="btn btn-sm btn-primary send-request-btn" data-id="${user._id}">Send Request</button>`;
    if (user.status === 'friend') {
        actionBtn = `<span class="status-text">Already Friends</span>`;
    } else if (user.status === 'pending') {
        actionBtn = `<span class="status-text">Request Sent</span>`;
    }
    return `
        <li class="user-list-item">
            <span class="user-display-name">${user.displayName}</span>
            <div class="user-actions">${actionBtn}</div>
        </li>
    `;
}

function renderPendingRequestItem(request) {
    const isIncoming = request.type === 'incoming';
    const statusTag = `<span class="status-tag ${isIncoming ? 'status-incoming' : 'status-outgoing'}">${request.type}</span>`;
    const actions = isIncoming
        ? `<button class="btn btn-sm btn-success respond-request-btn" data-id="${request._id}" data-action="accept">Accept</button>
           <button class="btn btn-sm btn-danger respond-request-btn" data-id="${request._id}" data-action="decline">Decline</button>`
        : `<button class="btn btn-sm btn-danger respond-request-btn" data-id="${request._id}" data-action="cancel">Cancel</button>`;
    return `
        <li class="user-list-item">
            <span class="user-display-name">${request.displayName}</span>
            <div class="user-actions">
                ${statusTag}
                ${actions}
            </div>
        </li>
    `;
}

// --- MAIN RENDER FUNCTION ---

export async function renderFriendsView(container) {
    container.innerHTML = `
        <h1>Friends</h1>
        <p class="page-description">Add, manage, and interact with your friends.</p>
        <div class="card friends-card">
            <h2>Add Friends</h2>
            <div class="form-group search-bar">
                <input type="text" id="user-search-input" class="form-input" placeholder="Enter display name to find users...">
                <button id="user-search-btn" class="btn btn-primary">Search</button>
            </div>
            <div id="user-search-results-container"></div>
        </div>
        <div class="card friends-card">
            <h2>My Friends (<span id="friends-count">0</span>)</h2>
             <div class="form-group search-bar">
                <input type="text" id="friend-filter-input" class="form-input" placeholder="Filter friends by name...">
            </div>
            <ul id="friends-list" class="user-list"></ul>
        </div>
        <div class="card friends-card">
            <h2>Pending Requests</h2>
            <ul id="pending-requests-list" class="user-list"></ul>
        </div>
    `;

    await loadAndRenderAllData();
    attachFriendsEventListeners();
}

// --- DATA HANDLING & RENDERING ---

async function loadAndRenderAllData() {
    try {
        const data = await getFriendsData();
        if (!data.success) throw new Error(data.message || 'Failed to load friends data.');
        allFriends = data.friends || [];
        document.getElementById('friends-count').textContent = allFriends.length;
        document.getElementById('friends-list').innerHTML = allFriends.length > 0
            ? allFriends.map(renderFriendListItem).join('')
            : '<li class="list-placeholder">You have no friends yet.</li>';
        const incoming = (data.incomingRequests || []).map(u => ({ ...u, type: 'incoming' }));
        const outgoing = (data.outgoingRequests || []).map(u => ({ ...u, type: 'outgoing' }));
        const allRequests = [...incoming, ...outgoing];
        document.getElementById('pending-requests-list').innerHTML = allRequests.length > 0
            ? allRequests.map(renderPendingRequestItem).join('')
            : '<li class="list-placeholder">No pending requests.</li>';
    } catch (error) {
        showNotification(error.message, true);
    }
}

function filterFriends() {
    const query = document.getElementById('friend-filter-input').value.toLowerCase();
    const filteredFriends = allFriends.filter(friend => friend.displayName.toLowerCase().includes(query));
    document.getElementById('friends-list').innerHTML = filteredFriends.length > 0
        ? filteredFriends.map(renderFriendListItem).join('')
        : '<li class="list-placeholder">No friends found matching your filter.</li>';
}

// --- EVENT LISTENERS & HANDLERS ---

function attachFriendsEventListeners() {
    const container = document.getElementById('main-content');
    document.getElementById('user-search-btn')?.addEventListener('click', handleUserSearch);
    document.getElementById('user-search-input')?.addEventListener('keyup', (e) => e.key === 'Enter' && handleUserSearch());
    document.getElementById('friend-filter-input')?.addEventListener('input', filterFriends);

    container.addEventListener('click', async (e) => {
        const target = e.target.closest('button');
        if (!target) return;

        const { id, action } = target.dataset;

        if (target.matches('.send-request-btn')) {
            target.disabled = true;
            target.textContent = 'Sending...';
            const data = await sendFriendRequest(id);
            if (data.success) target.textContent = 'Sent!';
            else {
                showNotification(data.message, true);
                target.disabled = false;
                target.textContent = 'Send Request';
            }
        } else if (target.matches('.respond-request-btn')) {
            target.closest('.user-actions').querySelectorAll('button').forEach(btn => btn.disabled = true);
            await respondToRequest(id, action);
            await loadAndRenderAllData();
        } else if (target.matches('.remove-friend-btn')) {
            // NOTE: The native confirm() dialog can be unreliable in this environment.
            // For now, removing friend directly. A custom modal would be a good future improvement.
            await removeFriend(id);
            await loadAndRenderAllData();
        } else if (action === 'inventory') {
            const userId = target.dataset.id;
            const userName = target.closest('.user-list-item').querySelector('.user-display-name').textContent;
            // This will navigate to the inventory view and show the selected friend's collection
            renderInventoryView(container, { userId, userName });
        } else if (['message', 'trade', 'battle'].includes(action)) {
            showNotification(`'${action}' button clicked. (Not implemented)`);
        }
    });
}

async function handleUserSearch() {
    const searchInput = document.getElementById('user-search-input');
    const resultsContainer = document.getElementById('user-search-results-container');
    const query = searchInput.value.trim();
    if (query.length < 3) {
        resultsContainer.innerHTML = `<p class="search-message">Enter at least 3 characters.</p>`;
        return;
    }
    resultsContainer.innerHTML = `<p class="search-message">Searching...</p>`;
    try {
        const data = await searchUsers(query);
        resultsContainer.innerHTML = data.success && data.users.length > 0
            ? `<ul class="user-list">${data.users.map(renderSearchResultItem).join('')}</ul>`
            : `<p class="search-message">${data.success ? 'No users found.' : data.message}</p>`;
    } catch (error) {
        resultsContainer.innerHTML = `<p class="search-message message-error">${error.message}</p>`;
    }
}

