import React from "react";

export function Card({ children, className = "" }) {
  return (
    <div
      className={`rounded-lg border border-neutral-700/60 bg-neutral-800/80 shadow
                  backdrop-blur-md ${className}`}
    >
      {children}
    </div>
  );
}

export function CardHeader({ children, className = "" }) {
  return (
    <header
      className={`border-b border-neutral-700/60 px-4 py-2 text-lg font-semibold
                  text-neutral-100 ${className}`}
    >
      {children}
    </header>
  );
}

export const CardContent = React.forwardRef(({ children, className = "" }, ref) => {
  return (
    <div ref={ref} className={`px-4 py-3 ${className}`}>
      {children}
    </div>
  );
});
CardContent.displayName = "CardContent";
