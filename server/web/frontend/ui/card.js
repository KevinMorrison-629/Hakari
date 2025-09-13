export default class CardUI {
    constructor(targetElement, cardData) {
        this.targetElement = targetElement;
        this.data = cardData;
        this.isDraggable = false;
        this.isDefaultBack = false;
        this.isFlipped = false;

        this.createElement();
        this.render();
    }

    createElement() {
        this.cardContainer = document.createElement('div');
        // Add card-component class and use the unique card ID for the element ID and dataset
        this.cardContainer.className = 'card-component card-container';
        this.cardContainer.id = `card-${this.data.id}`;
        this.cardContainer.dataset.cardId = this.data.id;

        this.cardContainer.innerHTML = `
            <div class="card-inner">
                <div class="card-face card-front border-4 border-slate-400">
                    <div class="p-2 bg-slate-300 text-slate-800 flex justify-between items-center border-b border-slate-400">
                        <span class="font-bold text-blue-800">${this.data.name}</span>
                        <span class="font-semibold text-slate-600">#${this.data.acqNumber.toString().padStart(3, '0')}</span>
                    </div>
                    <div class="flex-grow image-wrapper flex items-center justify-center p-2">
                        <div class="card-image-container">
                            <img src="${this.data.imageUrl}" alt="${this.data.name}" onerror="this.src='https://placehold.co/225x350/e2e8f0/334155?text=Image+Missing'" />
                        </div>
                    </div>
                    <div class="card-footer bg-slate-300 text-slate-800">
                        <div class="stat-box text-red-600">
                            <span>⚔️</span>
                            <span>${this.data.ap}</span>
                        </div>
                        <div class="ability-box">
                           ${this.data.ability || ''}
                        </div>
                        <div class="stat-box text-green-600">
                            <span>❤️</span>
                            <span>${this.data.hp}</span>
                        </div>
                    </div>
                </div>

                <div class="card-face card-back border-4 border-slate-400">
                     <h3 class="text-xl font-bold mb-4">${this.data.name}</h3>
                     <div class="flex flex-col gap-3 w-full px-4">
                        <a href="#" class="bg-slate-500 hover:bg-slate-600 text-white text-center font-bold py-2 px-4 rounded transition w-full">Card Reference</a>
                        <a href="#" class="bg-slate-500 hover:bg-slate-600 text-white text-center font-bold py-2 px-4 rounded transition w-full">Character Details</a>
                        <a href="#" class="bg-slate-500 hover:bg-slate-600 text-white text-center font-bold py-2 px-4 rounded transition w-full">Universe Page</a>
                     </div>
                </div>
            </div>
            <div class="card-default-back tier-${this.data.tier} hidden items-center justify-center">
                <svg xmlns="http://www.w3.org/2000/svg" width="80" height="80" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1" stroke-linecap="round" stroke-linejoin="round" class="opacity-50"><path d="M15 4V2a2 2 0 0 0-2-2H4a2 2 0 0 0-2 2v11a2 2 0 0 0 2 2h2"></path><path d="M22 13a2 2 0 0 0-2-2h-1a2 2 0 0 0-2 2v1a2 2 0 0 0 2 2h1a2 2 0 0 0 2-2v-1Z"></path><path d="M3 13a2 2 0 0 0-2-2h-1a2 2 0 0 0-2 2v1a2 2 0 0 0 2 2h1a2 2 0 0 0 2-2v-1Z"></path><path d="M12.5 13a2 2 0 0 0-2-2h-1a2 2 0 0 0-2 2v1a2 2 0 0 0 2 2h1a2 2 0 0 0 2-2v-1Z"></path></svg>
            </div>
        `;

        this.cardInner = this.cardContainer.querySelector('.card-inner');
        this.defaultBackElement = this.cardContainer.querySelector('.card-default-back');

        this.cardContainer.addEventListener('click', () => {
            if (!this.isDefaultBack) {
                this.flip();
            }
        });
    }

    flip() { this.isFlipped = !this.isFlipped; this.render(); }
    showFront() { this.isFlipped = false; this.isDefaultBack = false; this.render(); }
    showBack() { this.isFlipped = true; this.isDefaultBack = false; this.render(); }
    setDraggable(isDraggable) { this.isDraggable = isDraggable; this.render(); }
    setDefaultBack(isDefault) { this.isDefaultBack = isDefault; this.render(); }

    render() {
        this.cardContainer.classList.toggle('is-flipped', this.isFlipped);
        this.cardContainer.classList.toggle('is-draggable', this.isDraggable);

        // Set the draggable attribute for compatibility with drag-and-drop selectors
        if (this.isDraggable) {
            this.cardContainer.setAttribute('draggable', 'true');
        } else {
            this.cardContainer.removeAttribute('draggable');
        }

        if (this.isDefaultBack) {
            this.defaultBackElement.classList.remove('hidden');
            this.defaultBackElement.classList.add('flex');
            this.cardInner.classList.add('hidden');
        } else {
            this.defaultBackElement.classList.add('hidden');
            this.defaultBackElement.classList.remove('flex');
            this.cardInner.classList.remove('hidden');
        }

        if (!this.targetElement.contains(this.cardContainer)) {
            this.targetElement.appendChild(this.cardContainer);
        }
    }
}
