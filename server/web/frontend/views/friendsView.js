import { apiFetch } from '../api.js';

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

        <!-- Add New Friends -->
        <div class="card friends-card">
            <h2>Add Friends</h2>
            <div class="form-group search-bar">
                <input type="text" id="user-search-input" class="form-input" placeholder="Enter display name to find users...">
                <button id="user-search-btn" class="btn btn-primary">Search</button>
            </div>
            <div id="user-search-results-container"></div>
        </div>

        <!-- My Friends -->
        <div class="card friends-card">
            <h2>My Friends (<span id="friends-count">0</span>)</h2>
             <div class="form-group search-bar">
                <input type="text" id="friend-filter-input" class="form-input" placeholder="Filter friends by name...">
            </div>
            <ul id="friends-list" class="user-list"></ul>
        </div>

        <!-- Pending Requests -->
        <div class="card friends-card">
            <h2>Pending Requests</h2>
            <ul id="pending-requests-list" class="user-list"></ul>
        </div>
    `;

    await loadAndRenderAllData();
    attachFriendsEventListeners();
}

// --- DATA HANDLING AND RENDERING ---

async function loadAndRenderAllData() {
    try {
        const response = await apiFetch('/api/friends');
        const data = await response.json();

        if (!response.ok || !data.success) {
            throw new Error(data.message || 'Failed to load friends data.');
        }

        // Cache friends for filtering
        allFriends = data.friends || [];

        // Render Friends List
        const friendsListEl = document.getElementById('friends-list');
        const friendsCountEl = document.getElementById('friends-count');
        friendsCountEl.textContent = allFriends.length;
        if (allFriends.length > 0) {
            friendsListEl.innerHTML = allFriends.map(renderFriendListItem).join('');
        } else {
            friendsListEl.innerHTML = '<li class="list-placeholder">You have no friends yet. Use the search above to add some!</li>';
        }


        // Combine and Render Pending Requests
        const pendingListEl = document.getElementById('pending-requests-list');
        const incoming = (data.incomingRequests || []).map(u => ({ ...u, type: 'incoming' }));
        const outgoing = (data.outgoingRequests || []).map(u => ({ ...u, type: 'outgoing' }));
        const allRequests = [...incoming, ...outgoing];

        if (allRequests.length > 0) {
            pendingListEl.innerHTML = allRequests.map(renderPendingRequestItem).join('');
        } else {
            pendingListEl.innerHTML = '<li class="list-placeholder">No pending requests.</li>';
        }


    } catch (error) {
        console.error('Error loading friends data:', error);
        // You can add error messages to the UI here
    }
}

function filterFriends() {
    const query = document.getElementById('friend-filter-input').value.toLowerCase();
    const friendsListEl = document.getElementById('friends-list');

    const filteredFriends = allFriends.filter(friend => friend.displayName.toLowerCase().includes(query));

    if (filteredFriends.length > 0) {
        friendsListEl.innerHTML = filteredFriends.map(renderFriendListItem).join('');
    } else {
        friendsListEl.innerHTML = '<li class="list-placeholder">No friends found matching your filter.</li>';
    }
}


// --- EVENT LISTENERS ---

function attachFriendsEventListeners() {
    const container = document.getElementById('main-content');

    // Search for new users
    const userSearchBtn = document.getElementById('user-search-btn');
    const userSearchInput = document.getElementById('user-search-input');
    userSearchBtn.addEventListener('click', handleUserSearch);
    userSearchInput.addEventListener('keyup', (e) => e.key === 'Enter' && handleUserSearch());

    // Filter existing friends
    const friendFilterInput = document.getElementById('friend-filter-input');
    friendFilterInput.addEventListener('input', filterFriends);


    // --- Event Delegation for dynamic buttons ---
    container.addEventListener('click', async (e) => {
        const target = e.target.closest('button');
        if (!target) return;

        // Send Friend Request
        if (target.matches('.send-request-btn')) {
            target.disabled = true;
            target.textContent = 'Sending...';
            await handleSendRequest(target.dataset.id);
            // Don't need to reload all data, just update the button
            target.textContent = 'Sent!';
        }

        // Respond to or Cancel a Friend Request
        if (target.matches('.respond-request-btn')) {
            const userId = target.dataset.id;
            const action = target.dataset.action;
            // Disable all buttons for this request
            target.closest('.user-actions').querySelectorAll('button').forEach(btn => btn.disabled = true);
            await handleRespondToRequest(userId, action);
            await loadAndRenderAllData(); // Refresh everything
        }

        // Remove Friend
        if (target.matches('.remove-friend-btn')) {
            target.disabled = true;
            target.textContent = '✖';
            await handleRemoveFriend(target.dataset.id);
            await loadAndRenderAllData(); // Refresh everything
        }

        // Placeholder for other friend actions
        const friendAction = target.dataset.action;
        if (['inventory', 'message', 'trade', 'battle'].includes(friendAction)) {
            console.log(`Action: ${friendAction} on user ${target.dataset.id}`);
            // Here you would implement the logic for these buttons,
            // e.g., opening a new view or a modal.
            alert(`'${friendAction}' button clicked for user ID: ${target.dataset.id}. \n(Functionality not yet implemented.)`);
        }
    });
}

async function handleUserSearch() {
    const searchInput = document.getElementById('user-search-input');
    const searchBtn = document.getElementById('user-search-btn');
    const resultsContainer = document.getElementById('user-search-results-container');
    const query = searchInput.value.trim();

    if (query.length < 3) {
        resultsContainer.innerHTML = `<p class="search-message">Please enter at least 3 characters.</p>`;
        return;
    }

    searchBtn.disabled = true;
    searchBtn.innerHTML = `<span class="animate-spin">🔍</span>`;
    resultsContainer.innerHTML = `<p class="search-message">Searching...</p>`;

    try {
        const response = await apiFetch(`/api/users/search?name=${encodeURIComponent(query)}`);
        const data = await response.json();

        if (response.ok && data.success) {
            if (data.users.length > 0) {
                resultsContainer.innerHTML = `<ul class="user-list">${data.users.map(renderSearchResultItem).join('')}</ul>`;
            } else {
                resultsContainer.innerHTML = `<p class="search-message">No users found.</p>`;
            }
        } else {
            resultsContainer.innerHTML = `<p class="search-message message-error">${data.message || 'Search failed.'}</p>`;
        }
    } catch (error) {
        resultsContainer.innerHTML = `<p class="search-message message-error">An error occurred during search.</p>`;
    } finally {
        searchBtn.disabled = false;
        searchBtn.textContent = 'Search';
    }
}


// --- API WRAPPER FUNCTIONS ---

async function handleSendRequest(recipientId) {
    try {
        const res = await apiFetch('/api/friends/request', {
            method: 'POST',
            body: JSON.stringify({ recipientId })
        });
        const data = await res.json();
        if (!res.ok) alert(data.message); // Simple feedback for user
    } catch (error) {
        console.error('Failed to send friend request:', error);
    }
}

async function handleRespondToRequest(otherUserId, action) {
    try {
        await apiFetch('/api/friends/response', {
            method: 'POST',
            body: JSON.stringify({ otherUserId, action })
        });
    } catch (error) {
        console.error(`Failed to ${action} friend request:`, error);
    }
}

async function handleRemoveFriend(friendId) {
    // A simple confirmation dialog
    if (!confirm('Are you sure you want to remove this friend?')) return;
    try {
        await apiFetch(`/api/friends/${friendId}`, {
            method: 'DELETE'
        });
    } catch (error) {
        console.error('Failed to remove friend:', error);
    }
}

