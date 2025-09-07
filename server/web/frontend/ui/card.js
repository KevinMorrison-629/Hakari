/**
 * Renders a single card component.
 * @param {object} card - The card object with properties like id, image, name, and number.
 * @param {object} options - Configuration options for the card.
 * @param {boolean} options.isDraggable - Determines if the card should have the draggable attribute.
 * @returns {string} The HTML string for the card element.
 */
export function renderCard(card, options = { isDraggable: false }) {
    return `
        <div class="card-component" 
             draggable="${options.isDraggable}" 
             data-card-id="${card.id}"
             title="${card.name} - #${card.number}">
            <img src="${card.image}" alt="${card.name}" class="card-image" />
            <div class="card-info">
                <h3>${card.name}</h3>
                <p># ${card.number}</p>
            </div>
        </div>
    `;
}

