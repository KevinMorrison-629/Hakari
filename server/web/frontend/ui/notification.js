let notificationEl = null;
let notificationTimeout;

/**
 * Ensures the notification element and its styles are present in the DOM.
 */
function ensureNotificationElement() {
    if (notificationEl && document.body.contains(notificationEl)) {
        return;
    }

    const styleId = 'notification-styles';
    if (!document.getElementById(styleId)) {
        const style = document.createElement('style');
        style.id = styleId;
        style.innerHTML = `
            .notification-popup {
                position: fixed;
                top: 20px;
                left: 50%;
                transform: translateX(-50%);
                padding: 12px 24px;
                border-radius: 8px;
                color: white;
                font-size: 16px;
                font-weight: 500;
                z-index: 1000;
                opacity: 0;
                visibility: hidden;
                transition: opacity 0.3s, visibility 0.3s, top 0.3s ease-out;
                box-shadow: 0 4px 12px rgba(0,0,0,0.15);
            }
            .notification-popup.show {
                opacity: 1;
                visibility: visible;
                top: 40px;
            }
            .notification-popup.success {
                background-color: #28a745; /* Green */
            }
            .notification-popup.error {
                background-color: #dc3545; /* Red */
            }
        `;
        document.head.appendChild(style);
    }

    notificationEl = document.createElement('div');
    notificationEl.id = 'notification-popup';
    notificationEl.className = 'notification-popup';
    document.body.appendChild(notificationEl);
}

/**
 * Shows a pop-up notification message.
 * @param {string} message The message to display.
 * @param {boolean} isError True for red (error), false for green (success).
 */
export function showNotification(message, isError = false) {
    ensureNotificationElement();

    clearTimeout(notificationTimeout);

    notificationEl.textContent = message;
    notificationEl.className = 'notification-popup'; // Reset classes
    notificationEl.classList.add(isError ? 'error' : 'success');
    notificationEl.classList.add('show');

    notificationTimeout = setTimeout(() => {
        notificationEl.classList.remove('show');
    }, 3000);
}
