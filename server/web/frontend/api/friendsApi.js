import { apiFetch } from './api.js';

/**
 * Fetches all friends data including friends list, incoming, and outgoing requests.
 * @returns {Promise<object>} The friends data.
 */
export async function getFriendsData() {
    const response = await apiFetch('/api/friends');
    if (!response.ok) {
        const errorData = await response.json().catch(() => ({ message: 'Failed to load friends data.' }));
        throw new Error(errorData.message);
    }
    return response.json();
}

/**
 * Searches for users by display name.
 * @param {string} nameQuery The search query.
 * @returns {Promise<object>} The search results.
 */
export async function searchUsers(nameQuery) {
    const response = await apiFetch(`/api/users/search?name=${encodeURIComponent(nameQuery)}`);
    if (!response.ok) {
        const errorData = await response.json().catch(() => ({ message: 'Search failed.' }));
        throw new Error(errorData.message);
    }
    return response.json();
}

/**
 * Sends a friend request to another user.
 * @param {string} recipientId The ID of the user to send the request to.
 * @returns {Promise<object>} The response data.
 */
export async function sendFriendRequest(recipientId) {
    const res = await apiFetch('/api/friends/request', {
        method: 'POST',
        body: JSON.stringify({ recipientId })
    });
    return res.json();
}

/**
 * Responds to a friend request (accept, decline, cancel).
 * @param {string} otherUserId The ID of the other user in the request.
 * @param {string} action The action to take ('accept', 'decline', 'cancel').
 * @returns {Promise<Response>} The raw response object.
 */
export async function respondToRequest(otherUserId, action) {
    return await apiFetch('/api/friends/response', {
        method: 'POST',
        body: JSON.stringify({ otherUserId, action })
    });
}

/**
 * Removes a friend.
 * @param {string} friendId The ID of the friend to remove.
 * @returns {Promise<Response>} The raw response object.
 */
export async function removeFriend(friendId) {
    return await apiFetch(`/api/friends/${friendId}`, {
        method: 'DELETE'
    });
}
